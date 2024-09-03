#ifndef TEXTURE_H
#define TEXTURE_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef NOASS
	#include <assimp/scene.h>
#endif

#include "stb_image.h"
#include "Shader.h"

class Texture
{
public:
	GLuint ID;
	std::string type;
	GLuint unit;
	std::string path;

	Texture(const char* image, std::string texType, GLuint slot);
#ifndef NOASS
	Texture(const aiTexture* aiTex, std::string texType, GLuint slot);
#endif
	void print();
	void texUnit(Shader& shader, const char* uniform);
	void Bind();
	void Unbind();
	void Delete();
};

#endif
