#ifndef LIGHT_H
#define LIGHT_H

#include "glm/glm.hpp"
#include "Shader.h"
#include "PointShadow.h"

class Light {
	public:
		glm::vec4 lightColor;
		glm::vec3 lightPos;

		Light(glm::vec4 col, glm::vec3 pos) {
			lightColor = col;
			lightPos = pos;

			Light::lightShadow = new PointShadow(lightPos);
		}

		~Light() {
			delete lightShadow;
		}

		void linkShader(Shader sh) {
			sh.Activate();
			glUniform4f(glGetUniformLocation(sh.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
			glUniform3f(glGetUniformLocation(sh.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		}

		PointShadow * lightShadow;

        glm::vec3 ambient = glm::vec3(1.0f);
		glm::vec3 diffuse = glm::vec3(1.5f);
		glm::vec3 specular = glm::vec3(2.0f);

		float constant = 1.0f;
		float linear = 0.022;
		float quadratic = 0.0019;

	private:
};

#endif
