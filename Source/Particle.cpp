#include "Particle.h"

void Particle::update(float delta) {
	life += delta;
	float dt = -2.0f * sin(0.5f * life) + 2;

	glm::vec3 randVec = glm::normalize(glm::vec3(((float)rand() / RAND_MAX) - 0.5f, ((float)rand() / RAND_MAX) - 0.5f, ((float)rand() / RAND_MAX) - 0.5f)) * 0.03f;
	vel += randVec;
	vel.y += lift;
	
	translation = translation + vel * dt * delta;

	// the only one that looks good like this
	scale = scale + delta * scale.x * 1.1f;
	//scale = glm::vec3(scalar) + (glm::vec3(scalar) * 10.0f * sin(0.5f * life));
	
}

glm::vec3 Particle::getTranslation() {
	return translation;
}

void Particle::setTranslation(glm::vec3 translation) {
	Particle::translation = translation;
}

void Particle::setScale(float scalar) {
	Particle::scale = glm::vec3(scalar);
}