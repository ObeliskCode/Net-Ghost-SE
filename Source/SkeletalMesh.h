#ifndef SKELETAL_MESH_H
#define SKELETAL_MESH_H

#include <string>
#include <vector>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Globals.h"
#include "Texture.h"


class SkeletalMesh {
public:
	std::vector <SkeletalVertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures; // should be vector of pointers, but won't optimize much.
	glm::mat4 model; // sends mesh to default model position (same as in blender)

	SkeletalVAO m_VAO;
	SkeletalVBO m_VBO;
	EBO m_EBO;

	SkeletalMesh(std::vector <SkeletalVertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, glm::mat4& model);
	SkeletalMesh(SkeletalMesh&& other) {
        SkeletalMesh::vertices = std::move(other.vertices);
        SkeletalMesh::indices = std::move(other.indices);
        SkeletalMesh::textures = std::move(other.textures);
        SkeletalMesh::model = std::move(other.model);
        SkeletalMesh::m_VAO = std::move(other.m_VAO);
        other.m_VAO = SkeletalVAO();
        SkeletalMesh::m_VBO = std::move(other.m_VBO);
        other.m_VBO = SkeletalVBO();
        SkeletalMesh::m_EBO = std::move(other.m_EBO);
        other.m_EBO = EBO();
    }
	~SkeletalMesh();

	void Draw
	(
		Shader& shader,
		Camera& camera,
		glm::mat4 transform,
		glm::mat4 ntransform
	);


	void DrawShadow
	(
		Shader& shader,
		glm::mat4 transform
	);
};

#endif
