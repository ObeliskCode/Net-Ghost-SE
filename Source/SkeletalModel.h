#ifndef SKELETAL_MODEL_H
#define SKELETAL_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <unordered_map>

#include "SkeletalMesh.h"
#include "Material.h"

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
        glm::mat4& transform, glm::mat4& ntransform);

    void DrawShadow(Shader& shader, glm::mat4& transform);

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }

    Material mat;

private:
    // model data
    std::vector<SkeletalMesh*> meshes;

    std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4t<float> transformation, std::string directory, std::vector<Texture>& textures_loaded);
    SkeletalMesh* processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4t<float>& transformation, std::string directory, std::vector<Texture>& textures_loaded);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type,
        std::string typeName, int slotInc, std::string directory, std::vector<Texture>& textures_loaded);
    glm::mat4 aiMat4toGLM(aiMatrix4x4t<float>& matrix);

    void ExtractBoneWeightForVertices(std::vector<SkeletalVertex>& vertices, aiMesh* mesh, const aiScene* scene);
};

#endif
