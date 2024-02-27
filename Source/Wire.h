#ifndef WIRE_H
#define WIRE_H

#include "Camera.h"
#include "glm/glm.hpp"
#include <vector>

class Wire {
	public:
		Wire(glm::vec3 x_0, glm::vec3 x_1);
		void Draw(Shader& shader, Camera& camera);

	private:
		std::vector <float> Vert;
		std::vector <GLuint> Ind;
		GLuint VAO, VBO, EBO;
};

#endif
