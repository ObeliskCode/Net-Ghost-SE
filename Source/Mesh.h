#ifndef MESH_H
#define MESH_H


#include <string>
#include <vector>

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"


class Mesh {
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures; // texture class is already sudo-pointer
	glm::mat4 model; // sends mesh to default model position (same as in blender) AKA model matrix

	VAO* m_VAO;
	VBO* m_VBO;
	EBO* m_EBO;

	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, glm::mat4& transformation);
	~Mesh();

	void Draw
	(
		Shader& shader,
		Camera& camera,
		glm::vec3& translation,
		glm::quat& rotation,
		glm::vec3& scale
	);
};

#endif
