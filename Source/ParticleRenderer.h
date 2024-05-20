#ifndef PARTICLERENDERER_H
#define PARTICLERENDERER_H

#include <vector>
#include "glm/glm.hpp"
#include "Texture.h"
#include "Particle.h"

// class for instancing particles 

class ParticleRenderer {
	public:
		ParticleRenderer();
		~ParticleRenderer();
		void DrawParticles(Shader& shader, Camera& camera);
		void updateParticles(float delta);

		std::vector <Particle> particles;

	private:
		std::vector <float> Vert;
		std::vector <GLuint> Ind;
		GLuint VAO, VBO, EBO;

		GLuint IVBO;
		GLuint IlVBO;

		Texture* sprite;

};


#endif