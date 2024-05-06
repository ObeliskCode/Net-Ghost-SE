#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm/glm.hpp"
#include "Shader.h"

class Material {
	public:
		Material(
			glm::vec3 ambient = glm::vec3(0.2f),
			glm::vec3 diffuse = glm::vec3(0.5f),
			glm::vec3 specular = glm::vec3(1.0f),
			float shininess = 16.0f) {
			Material::ambient = ambient;
			Material::diffuse = diffuse;
			Material::specular = specular;
			Material::shininess = shininess;
		}
		~Material() = default;

		void linkShader(Shader& sh) {
			glUniform3f(glGetUniformLocation(sh.ID, "material.ambient"), ambient.x, ambient.y, ambient.z);
			glUniform3f(glGetUniformLocation(sh.ID, "material.diffuse"), diffuse.x, diffuse.y, diffuse.z);
			glUniform3f(glGetUniformLocation(sh.ID, "material.specular"), specular.x, specular.y, specular.z);
			glUniform1f(glGetUniformLocation(sh.ID, "material.shininess"), shininess);
		}
	private:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
};

#endif