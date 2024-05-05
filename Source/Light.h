#ifndef LIGHT_H
#define LIGHT_H

#include "glm/glm.hpp"
#include "Shader.h"
#include "PointShadow.h"

class Light {
	public:
		glm::vec4 lightColor;
		glm::vec3 lightPos;

		Light(glm::vec4 col, glm::vec3 pos);
		~Light();

		void linkShader(Shader sh);

		PointShadow * lightShadow;

	private:
		glm::vec3 ambient = glm::vec3(0.2f);
		glm::vec3 diffuse = glm::vec3(0.5f);
		glm::vec3 specular = glm::vec3(1.0f);
};

#endif