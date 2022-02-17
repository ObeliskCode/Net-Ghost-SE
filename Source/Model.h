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
    void Draw(Shader& shader, Camera& camera);

    void setMatrix(glm::mat4 matrix);
    void setTranslation(glm::vec3 translation);
    void setRotation(glm::quat rotation);
    void setScale(glm::vec3 scale);

private:
    // model data
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;

    glm::mat4 matrix = glm::mat4(1.0f);
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4t<float> transformation);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4t<float> transformation);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        std::string typeName, int slotInc);
    glm::mat4 aiMat4toGLM(aiMatrix4x4t<float> matrix);
};
#endif