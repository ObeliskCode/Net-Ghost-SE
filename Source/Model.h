#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

class Model
{
public:
    Model(const char* path)
    {
        loadModel(path);
    }
    ~Model();
    void Draw(Shader& shader, Camera& camera);

    void setTranslation(glm::vec3 translation);
    void setRotation(glm::quat rotation);
    void setScale(glm::vec3 scale);

private:
    // model data
    std::vector<Texture> textures_loaded; // all textures
    std::vector<Mesh*> meshes;
    std::string directory;
    std::string fileType;

    // this is bad for instancing!? should be kept in a higher level class like Entity
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4t<float> transformation);
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4t<float>& transformation);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type,
        std::string typeName, int slotInc);
    glm::mat4 aiMat4toGLM(aiMatrix4x4t<float>& matrix);
};

#endif