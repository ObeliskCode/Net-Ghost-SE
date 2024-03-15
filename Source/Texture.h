#ifndef TEXTURE_H
#define TEXTURE_H

// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <assimp/scene.h>

#include "stb_image.h"
#include"shader.h"

class Texture
{
public:
	GLuint ID;
	std::string type;
	GLuint unit;
	std::string path;

	Texture(const char* image, std::string texType, GLuint slot);
	Texture(const aiTexture* aiTex, std::string texType, GLuint slot);

	void print();
	void texUnit(Shader& shader, const char* uniform);
	void Bind();
	void Unbind();
	void Delete();
};

#endif