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
    void Draw(Shader& shader, Camera& camera,
        glm::vec3& translation,
        glm::quat& rotation,
        glm::vec3& scale);

    void setUnitConversion(float uc);
    void setOffset(glm::vec3 offset);
    void setOrientation(glm::quat orientation);

private:
    // model data
    std::vector<Texture> textures_loaded; // all textures for all meshes
    std::vector<Mesh*> meshes;
    std::string directory;
    std::string fileType;

    glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
    float unitConversion = 1.0f;
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4t<float> transformation);
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4t<float>& transformation);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type,
        std::string typeName, int slotInc);
    glm::mat4 aiMat4toGLM(aiMatrix4x4t<float>& matrix);
};

#endif