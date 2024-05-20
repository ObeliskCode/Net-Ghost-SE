#ifndef QUADRENDERER_H
#define QUADRENDERER_H

#include <vector>
#include "glm/glm.hpp"
#include "Texture.h"
#include "Camera.h"
#include "Transform.h"

class Quad {
public:
    Transform t = Transform();
};

class QuadRenderer {
	public:
		QuadRenderer() {
            Vert.push_back(1.0f);
            Vert.push_back(1.0f);
            Vert.push_back(0.0f);

            Vert.push_back(0.0f);
            Vert.push_back(0.0f);

            Vert.push_back(-1.0f);
            Vert.push_back(1.0f);
            Vert.push_back(0.0f);

            Vert.push_back(1.0f);
            Vert.push_back(0.0f);

            Vert.push_back(-1.0f);
            Vert.push_back(-1.0f);
            Vert.push_back(0.0f);

            Vert.push_back(1.0f);
            Vert.push_back(1.0f);

            Vert.push_back(1.0f);
            Vert.push_back(-1.0f);
            Vert.push_back(0.0f);

            Vert.push_back(0.0f);
            Vert.push_back(1.0f);

            Ind.push_back(0);
            Ind.push_back(1);
            Ind.push_back(2);

            Ind.push_back(0);
            Ind.push_back(2);
            Ind.push_back(3);

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &IVBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, Vert.size() * sizeof(float), Vert.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, Ind.size() * sizeof(GLuint), Ind.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);
            glEnableVertexAttribArray(5);
            glBindBuffer(GL_ARRAY_BUFFER, IVBO);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
            glVertexAttribDivisor(2, 1);
            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            sprite = new Texture("models/smoke/smoke.png", "diffuse", 0);
		}

        ~QuadRenderer() {
            sprite->Delete();
            delete sprite;
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            quads.clear();
        }

        void DrawQuads(Shader& shader, Camera& camera) {
            std::vector <glm::mat4> transforms;

            shader.Activate();
            glBindVertexArray(VAO);

            sprite->texUnit(shader, "sprite");
            sprite->Bind();

            camera.Matrix(shader, "camMatrix");

            for (int i = 0; i < quads.size(); i++) {
                transforms.push_back(quads[i].t.getMatrix());
            }

            glBindBuffer(GL_ARRAY_BUFFER, IVBO);
            glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_STATIC_DRAW);

            glDrawElementsInstanced(GL_TRIANGLES, Ind.size(), GL_UNSIGNED_INT, 0, quads.size());

        }

		std::vector <Quad> quads;
	private:
		std::vector <float> Vert;
		std::vector <GLuint> Ind;
		GLuint VAO, VBO, EBO;

        GLuint IVBO;

        Texture* sprite;

};


#endif