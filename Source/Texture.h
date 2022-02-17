#ifndef TEXTURE_H
#define TEXTURE_H

// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

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

	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	void Bind();
	void Unbind();
	void Delete();
};
#endif