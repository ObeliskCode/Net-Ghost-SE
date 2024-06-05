#ifndef VBO_H
#define VBO_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "glm/glm.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texUV;
};


class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// undefined behavior if using default constructor obj!
	VBO() {};
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(std::vector<Vertex>& vertices);

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#define MAX_BONE_INFLUENCE 4

struct SkeletalVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texUV;
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

class SkeletalVBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// undefined behavior if using default constructor obj!
    SkeletalVBO() {};
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	SkeletalVBO(std::vector<SkeletalVertex>& vertices);

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#endif
