#ifndef WIRE_H
#define WIRE_H

#include "Camera.h"
#include "glm/glm.hpp"
#include <vector>

class Wire
{
public:
	Wire(glm::vec3 x_0, glm::vec3 x_1);
	~Wire();
	void Draw(Shader &shader, Camera &camera,
			  glm::mat4 transform);

private:
	std::vector<float> Vert;
	std::vector<GLuint> Ind;
	GLuint VAO, VBO, EBO;
};

#endif
