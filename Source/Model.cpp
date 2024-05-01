#include "Model.h"

Model::~Model() {
    for (unsigned int i = 0; i < textures_loaded.size(); i++)
    {
        textures_loaded[i].Delete();
    }
    textures_loaded.clear();
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        delete meshes[i];
    }
    meshes.clear();
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    path = "models/" + path;
    //const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);

    /* some models need aiProcess_FlipUVs and some don't.
       note: gltf files and dae files converted from gltf files don't require FlipUVs since STB already flips images
       so any models that come from gltf file format don't require FlipUVs
    */

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    fileType = path.substr(path.find_last_of('.'), path.back());

    aiMatrix4x4t<float> identity = aiMatrix4x4t<float>();
    processNode(scene->mRootNode, scene, identity);
}

void Model::processNode(aiNode* node, const aiScene* scene, aiMatrix4x4t<float> pTransform)
{
    aiMatrix4x4t<float> transform = pTransform * node->mTransformation;

    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, transform));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, transform);
    }
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4t<float>& transformation)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    glm::mat4 transform = aiMat4toGLM(transformation);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texUV = vec;
        }
        else
            vertex.texUV = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // process indices
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        int slotInc = 0;

        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, scene,
            aiTextureType_DIFFUSE, "diffuse", slotInc);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        
        slotInc = slotInc + diffuseMaps.size();

        std::vector<Texture> specularMaps = loadMaterialTextures(material, scene,
            aiTextureType_SPECULAR, "specular", slotInc);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return new Mesh(vertices, indices, textures, transform);
}

void Model::Draw(Shader& shader, Camera& camera,
    glm::mat4& transform, glm::mat4& ntransform){
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i]->Draw(shader, camera, transform, ntransform);
    } 
}

void Model::DrawShadow(Shader& shader, glm::mat4& transform){
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i]->DrawShadow(shader, transform);
    } 
}

// POSSIBLY CHANGE type in Texture from string to aiTextureType?


// BUG: .type member becomes corrupted when leaving loadMaterialTexture
// not really sure why adding int slotInc(rement) makes this work perfectly, was having issue where specular sampler was overriding diffuse sampler
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type, std::string typeName, int slotInc)
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        const char* c_str = str.C_Str();
        std::string path = directory + "/" + c_str;

        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), path.c_str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            if (fileType == ".fbx") {
                Texture texture = Texture(scene->GetEmbeddedTexture(path.c_str()), typeName, i + slotInc);
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
            else {
                Texture texture = Texture(path.c_str(), typeName, i + slotInc);
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }

        }
    }

    return textures;
}

glm::mat4 Model::aiMat4toGLM(aiMatrix4x4t<float>& matrix) {

    glm::mat4 m = glm::mat4(0.0f);
    m[0][0] = matrix.a1;
    m[0][1] = matrix.b1;
    m[0][2] = matrix.c1;
    m[0][3] = matrix.d1;

    m[1][0] = matrix.a2;
    m[1][1] = matrix.b2;
    m[1][2] = matrix.c2;
    m[1][3] = matrix.d2;

    m[2][0] = matrix.a3;
    m[2][1] = matrix.b3;
    m[2][2] = matrix.c3;
    m[2][3] = matrix.d3;

    m[3][0] = matrix.a4;
    m[3][1] = matrix.b4;
    m[3][2] = matrix.c4;
    m[3][3] = matrix.d4;

    return m;
}