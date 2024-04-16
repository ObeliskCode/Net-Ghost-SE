#include"Camera.h"

Camera::Camera(int width, int height, glm::vec3 position, float FOVdeg, float nearPlane, float farPlane)
{
	Camera::width = width;
	Camera::height = height;
	Camera::Position = position;
	Camera::FOVdeg = FOVdeg;
	Camera::nearPlane = nearPlane;
	Camera::farPlane = farPlane;
	_stale = true;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	if (_stale) {
		// Initializes matrices since otherwise they will be the null matrix
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// Makes camera look in the right direction from the right position
		view = glm::lookAt(Position, Position + Orientation, Up);
		// Adds perspective to the scene
		projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

		camMatrix = projection * view;
		_stale = false;
	}

	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(camMatrix));
}

void Camera::setDims(int width, int height) {
	_stale = true;
	Camera::width = width;
	Camera::height = height;
}

void Camera::setFrustrum(float FOVdeg, float nearPlane, float farPlane) {
	_stale = true;
	Camera::FOVdeg = FOVdeg;
	Camera::nearPlane = nearPlane;
	Camera::farPlane = farPlane;
}

void Camera::setPosition(glm::vec3 position) {
	_stale = true;
	Camera::Position = position;
}

void Camera::setOrientation(glm::vec3 Orientation) {
	_stale = true;
	Camera::Orientation = Orientation;
}