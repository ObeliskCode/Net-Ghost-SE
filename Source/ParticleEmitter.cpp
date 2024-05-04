#include "ParticleEmitter.h"

const double PI = 3.1415926535897932384626433832795028841971693993751058209;

ParticleEmitter::ParticleEmitter(){

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
    glGenBuffers(1, &IlVBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vert.size() * sizeof(float), Vert.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Ind.size() * sizeof(GLuint), Ind.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 *sizeof(float)));
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

    glEnableVertexAttribArray(6);
    glBindBuffer(GL_ARRAY_BUFFER, IlVBO);
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    sprite = new Texture("models/smoke/smoke.png", "diffuse", 0);

}

ParticleEmitter::~ParticleEmitter() {
    sprite->Delete();
    delete sprite;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &IVBO);
    glDeleteBuffers(1, &IlVBO);
    particles.clear();
}

void ParticleEmitter::DrawParticles(Shader& shader, Camera& camera){
    std::vector <glm::mat4> transforms;
    std::vector <float> lifeVec;

    shader.Activate();
	glBindVertexArray(VAO);

	sprite->texUnit(shader, "sprite");
	sprite->Bind();

	camera.Matrix(shader, "camMatrix");

    for (int i = 0; i < particles.size(); i++){
        glm::vec3 vec2cam = camera.getPosition() - particles[i].getTranslation();
        vec2cam.y = 0.f;
        vec2cam = glm::normalize(vec2cam);

        glm::vec3 orientation = glm::vec3(0.0f,0.0f,1.0f);
        orientation = glm::normalize(orientation);

        glm::vec3 crossProd = glm::cross(orientation, vec2cam);
        float w = sqrt(powf(glm::length(orientation),2)*powf(glm::length(vec2cam), 2)) + glm::dot(orientation, vec2cam);

        glm::quat rotation = glm::quat(w,crossProd);

        float len = glm::length(rotation);

        rotation.x = rotation.x / len;
        rotation.y = rotation.y / len;
        rotation.z = rotation.z / len;
        rotation.w = rotation.w / len;

        glm::vec3 vec2cam2 = camera.getPosition() - particles[i].getTranslation();
        vec2cam2 = glm::normalize(vec2cam2);

        crossProd = glm::cross(vec2cam, vec2cam2);
        w = sqrt(powf(glm::length(vec2cam), 2) * powf(glm::length(vec2cam2), 2)) + glm::dot(vec2cam, vec2cam2);

        glm::quat rotation2 = glm::quat(w, crossProd);

        len = glm::length(rotation2);

        rotation2.x = rotation2.x / len;
        rotation2.y = rotation2.y / len;
        rotation2.z = rotation2.z / len;
        rotation2.w = rotation2.w / len;

        float theta = particles[i].rotMag * 2.0f * PI/particles[i].expire;
        float q1 = cos(theta / 2.0f);
        float q2 = vec2cam2.x * sin(theta / 2.0f);
        float q3 = vec2cam2.y * sin(theta / 2.0f);
        float q4 = vec2cam2.z * sin(theta / 2.0f);
        glm::quat rotation3 = glm::quat(q1, q2, q3, q4);

        glm::quat rotCombine = rotation3 * rotation2 * rotation;

        // Transform the matrices to their correct form
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), particles[i].getTranslation());
        glm::mat4 rot = glm::mat4_cast(rotCombine);
        glm::mat4 sca = glm::scale(glm::mat4(1.0f), particles[i].scale);

        transforms.push_back(trans * rot * sca);
    }

    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(glm::mat4), transforms.data(), GL_STATIC_DRAW);

    for (int i = 0; i < particles.size(); i++){
        lifeVec.push_back(particles[i].life);
    }

    glBindBuffer(GL_ARRAY_BUFFER, IlVBO);
    glBufferData(GL_ARRAY_BUFFER, lifeVec.size() * sizeof(float), lifeVec.data(), GL_STATIC_DRAW);

	glDrawElementsInstanced(GL_TRIANGLES, Ind.size(), GL_UNSIGNED_INT, 0, particles.size());

}

void ParticleEmitter::updateParticles(float delta){
    for (int i = 0; i < particles.size(); i++){
        particles[i].update(delta);
    }
}