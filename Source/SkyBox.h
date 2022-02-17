#ifndef SKYBOX_H
#define SKYBOX_H

#include "Camera.h"
#include "stb_image.h"

class SkyBox {
	public:
		int skyWidth, skyHeight;
		unsigned int skyboxVAO;
		unsigned int cubemapTexture;

		SkyBox(Shader& shader, int skyWidth, int skyHeight);

		void Draw(Shader& shader, Camera& camera);

};

#endif