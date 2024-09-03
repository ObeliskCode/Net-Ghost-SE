#ifndef VAO_H
#define VAO_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "VBO.h"

class VAO
{
public:
	// ID reference for the Vertex Array Object
	GLuint ID;
	// Constructor that generates a VAO ID
	VAO();
/*
	VAO(VAO&& other) {
        VAO::ID = std::move(other.ID);
	}
	VAO& operator=(VAO&& other){
        this->ID = std::move(other.ID);
		return *this;
	}
*/
	// Links a VBO Attribute such as a position or color to the VAO
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	// Binds the VAO
	void Bind();
	// Unbinds the VAO
	void Unbind();
	// Deletes the VAO
	void Delete();
};


class SkeletalVAO
{
public:
	// ID reference for the Vertex Array Object
	GLuint ID;
	// Constructor that generates a VAO ID
	SkeletalVAO();
/*
	SkeletalVAO(SkeletalVAO&& other) {
        SkeletalVAO::ID = std::move(other.ID);
	}
	SkeletalVAO& operator=(SkeletalVAO&& other){
        this->ID = std::move(other.ID);
		return *this;
	}
*/
	// Links a VBO Attribute such as a position or color to the VAO
	void LinkAttrib(SkeletalVBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	// Binds the VAO
	void Bind();
	// Unbinds the VAO
	void Unbind();
	// Deletes the VAO
	void Delete();
};

#endif
