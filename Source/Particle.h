#ifndef PARTICLE_H
#define PARTICLE_H

#include "Camera.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include <vector>
#include <math.h>

class Particle {
	public:
		Particle() = default;

		void update(float delta);

		float expire = 3.0f;
		float lift = 0.006f;
		float life = 0.0f;
		
		glm::vec3 vel = glm::vec3(1.f,0.f,0.f);
		float rotMag = ((float)rand() / RAND_MAX) - 0.5f;
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

		glm::vec3 getTranslation();
		void setTranslation(glm::vec3 translation);
		void setScale(float scalar);

	private:

		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
};

#endif