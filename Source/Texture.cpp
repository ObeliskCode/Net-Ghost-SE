#include"Texture.h"

Texture::Texture(const aiTexture* aiTex, std::string texType, GLuint slot) {

	type = texType;

	//sets path to filepath input
	path = aiTex->mFilename.C_Str();

	int widthImg, heightImg, numColCh;
	unsigned char* bytes;

	//flip image setting
	stbi_set_flip_vertically_on_load(true);

	if (aiTex->mHeight == 0)
	{
		bytes = stbi_load_from_memory(reinterpret_cast<const uint8_t*>(aiTex->pcData), aiTex->mWidth, &widthImg, &heightImg, &numColCh, 0);
	}
	else
	{   // shouldn't it already be in memory?
		bytes = stbi_load_from_memory(reinterpret_cast<const uint8_t*>(aiTex->pcData), aiTex->mWidth * aiTex->mHeight, &widthImg, &heightImg, &numColCh, 0);
	}

	//generate 1 new texture ID and set to ID
	glGenTextures(1, &ID);
	//select active texture unit GL_TEXTURE(Slot)
	glActiveTexture(GL_TEXTURE0 + slot);
	//set texture "unit" to slot
	unit = slot;
	//bind texture to its target
	glBindTexture(GL_TEXTURE_2D, ID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (numColCh == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	}
	else if (numColCh == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	}

	//generate mip map (what does this do?)
	glGenerateMipmap(GL_TEXTURE_2D);

	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const char* image, std::string texType, GLuint slot)
{
	type = texType;

	//sets path to filepath input
	path = std::string(image);

	int widthImg, heightImg, numColCh;

	std::cout << "b4load" << std::endl;
	std::cout << path << std::endl;
	//flip image setting
	stbi_set_flip_vertically_on_load(true);
	//load bytes in and widthImg, heightImg and numColCh
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	if (stbi_failure_reason())
		std::cout << stbi_failure_reason() << std::endl;

	std::cout << "afterload" << std::endl;

	//generate 1 new texture ID and set to ID
	glGenTextures(1, &ID);
	//select active texture unit GL_TEXTURE(Slot)
	glActiveTexture(GL_TEXTURE0 + slot);
	//set texture "unit" to slot
	unit = slot;
	//bind texture to its target
	glBindTexture(GL_TEXTURE_2D, ID);

	//additional texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::cout << numColCh << std::endl;

	//load bytes into shader depending number of color channels
	if (numColCh == 4)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 3) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bytes
		);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	else if (numColCh == 1)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else
		throw std::invalid_argument("Automatic Texture type recognition failed");
	
	std::cout << "ok" << std::endl;

	//generate mip map (what does this do?)
	glGenerateMipmap(GL_TEXTURE_2D);

	//free bytes
	stbi_image_free(bytes);

	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

// get rid of unit parameter
void Texture::texUnit(Shader& shader, const char* uniform)
{
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);

	shader.Activate();

	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}

void Texture::print() 
{
	std::cerr << "ID " << ID << " type " << type << " unit " << unit << " path " << path << std::endl;
}