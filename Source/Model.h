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
        glm::mat4& transform, glm::mat4& ntransform);

    void DrawShadow(Shader& shader, glm::mat4& transform);

private:
    // model data
    std::vector<Texture> textures_loaded; // all textures for all meshes
    std::vector<Mesh*> meshes;
    std::string directory;
    std::string fileType;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4t<float> transformation);
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4t<float>& transformation);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, const aiScene* scene, aiTextureType type,
        std::string typeName, int slotInc);
    glm::mat4 aiMat4toGLM(aiMatrix4x4t<float>& matrix);
};

#endif