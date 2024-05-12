#ifndef CAMERA_H
#define CAMERA_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "Shader.h"

class Camera
{
public:
	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position, float FOVdeg, float nearPlane, float farPlane);

	// Updates and exports the camera matrix to the Vertex Shader
	void Matrix(Shader& shader, const char* uniform);

	void setDims(int width, int height);
	void setFrustrum(float FOVdeg, float nearPlane, float farPlane);
	void setPosition(glm::vec3 position);
	void setOrientation(glm::vec3 Orientation);

	glm::vec3 getPosition() {
		return Position;
	};

	glm::vec3 getOrientation() {
		return Orientation;
	};

	glm::vec3 getUp() {
		return Up;
	};

private:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 camMatrix;

	int width;
	int height;

	bool _stale;

	float FOVdeg;
	float nearPlane;
	float farPlane;
};

#endif
