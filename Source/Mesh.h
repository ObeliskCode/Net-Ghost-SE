#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Globals.h"
#include "Texture.h"


class Mesh {
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures; // texture class is already sudo-pointer
	glm::mat4 model; // sends mesh to default model position (same as in blender) AKA model matrix

	VAO m_VAO;
	VBO m_VBO;
	EBO m_EBO;

	Mesh(std::vector<Vertex> verts, std::vector<GLuint> indices);
	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, glm::mat4& model);
/*
	Mesh(Mesh&& other) {
		Mesh::vertices = std::move(other.vertices);
		Mesh::indices = std::move(other.indices);
		Mesh::textures = std::move(other.textures);
		Mesh::model = std::move(other.model);
		Mesh::m_VAO = std::move(other.m_VAO);
		other.m_VAO = VAO();
		Mesh::m_VBO = std::move(other.m_VBO);
		other.m_VBO = VBO();
		Mesh::m_EBO = std::move(other.m_EBO);
		other.m_EBO = EBO();
	}
*/
	~Mesh();

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
