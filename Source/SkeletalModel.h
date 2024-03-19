#ifndef SKELETAL_MODEL_H
#define SKELETAL_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <map>

#include "SkeletalMesh.h"

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    //WARNING: default value is 0.0f matrix
    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;

};

class SkeletalModel
{
public:
    SkeletalModel(const char* path)
    {
        loadModel(path);
    }
    ~SkeletalModel();
    void Draw(Shader& shader, Camera& camera,
        glm::vec3& translation,
        glm::quat& rotation,
        glm::vec3& scale);

    void setUnitConversion(float uc);
    void setOffset(glm::vec3 offset);
    void setOrientation(glm::quat orientation);

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

private:
    // model data
    std::vector<Texture> textures_loaded; // all textures
    std::vector<SkeletalMesh*> meshes;
    std::string directory;
    std::string fileType;

    glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
    float unitConversion = 1.0f;
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4t<float> transformation);
    SkeletalMesh* processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4t<float>& transformation);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type,
        std::string typeName, int slotInc);
    glm::mat4 aiMat4toGLM(aiMatrix4x4t<float>& matrix);

    void ExtractBoneWeightForVertices(std::vector<SkeletalVertex>& vertices, aiMesh* mesh, const aiScene* scene);
};

#endif