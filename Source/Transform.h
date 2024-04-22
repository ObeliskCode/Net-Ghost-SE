#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "glm/glm.hpp"

class Transform {
public:
	Transform() {
		_stale = true;
		_nstale = true;
		transform = getMatrix();
		ntransform = getNormalMatrix();
	}
	glm::mat4 transform;
	glm::mat4 ntransform;

	bool _stale;
	bool _nstale;

	void setTranslation(glm::vec3 trans) {
		_stale = true;
		translation = trans;
	}

	void setRotation(glm::quat rot) {
		_stale = true;
		_nstale = true;
		rotation = rot;
	}

	void setScale(glm::vec3 sca) {
		_stale = true;
		_nstale = true;
		scale = sca;
	}

	glm::vec3 getTranslation() {
		return translation;
	}

	glm::quat getRotation() {
		return rotation;
	}

	glm::vec3 getScale() {
		return scale;
	}


	glm::mat4 getMatrix() {
		if (_stale) {
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
			glm::mat4 rot = glm::mat4_cast(rotation);
			glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

			glm::mat4 tran = trans * rot * sca;

			_stale = false;
			transform = tran;
			return tran;
		}
		else {
			return transform;
		}
	}

	glm::mat4 getNormalMatrix() {
		if (_nstale) {
			glm::mat4 rot = glm::mat4_cast(rotation);
			glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

			glm::mat4 tran = rot * sca;

			_nstale = false; 
			ntransform = tran;
			return tran;
		}
		else {
			return ntransform;
		}
	}

private:
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

#endif