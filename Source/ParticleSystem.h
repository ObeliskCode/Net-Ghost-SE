#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include "glm/glm.hpp"
#include "Texture.h"
#include "Particle.h"

// class for instancing particles 

class ParticleSystem {
	public:
		ParticleSystem();
		~ParticleSystem();
		void DrawParticles(Shader& shader, Camera& camera);
		void updateParticles(float delta);

	private:
		std::vector <float> Vert;
		std::vector <GLuint> Ind;
		GLuint VAO, VBO, EBO;

		GLuint IVBO;
		GLuint IlVBO;

		Texture* sprite;

		std::vector <Particle> particles;

		std::vector <glm::mat4> transforms;
		std::vector <float> lifeVec;
};


#endif