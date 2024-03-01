#ifndef WIRE_H
#define WIRE_H

#include "Camera.h"
#include "glm/glm.hpp"
#include <vector>

class Wire {
	public:
		Wire(glm::vec3 x_0, glm::vec3 x_1);
		//~Wire() TODO
		void Draw(Shader& shader, Camera& camera);

		void setTranslation(glm::vec3 translation);
		void setRotation(glm::quat rotation);
		void setScale(glm::vec3 scale);

	private:
		std::vector <float> Vert;
		std::vector <GLuint> Ind;
		GLuint VAO, VBO, EBO;

		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

#endif
