#include "SkeletalMesh.h"
#include <glm/gtx/string_cast.hpp>

SkeletalMesh::SkeletalMesh(std::vector <SkeletalVertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, glm::mat4& model) {

	SkeletalMesh::vertices = vertices;
	SkeletalMesh::indices = indices;
	SkeletalMesh::textures = textures;
	SkeletalMesh::model = model;

	m_VAO = new SkeletalVAO();

	m_VAO->Bind();

	m_VBO = new SkeletalVBO(vertices);
	m_EBO = new EBO(indices);

	m_VAO->LinkAttrib(*m_VBO, 0, 3, GL_FLOAT, sizeof(SkeletalVertex), (void*)0);
	m_VAO->LinkAttrib(*m_VBO, 1, 3, GL_FLOAT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, normal));
	m_VAO->LinkAttrib(*m_VBO, 2, 2, GL_FLOAT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, texUV));

	m_VBO->Bind();
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, m_BoneIDs)); //AttribI function used instead!
	glEnableVertexAttribArray(3);
	m_VBO->Unbind();

	m_VAO->LinkAttrib(*m_VBO, 4, 4, GL_FLOAT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, m_Weights));

	m_VAO->Unbind();
	m_VBO->Unbind(); // m_VBO already unbinded by LinkAttrib()
	m_EBO->Unbind();
}

SkeletalMesh::~SkeletalMesh() {
	m_VAO->Delete();
	delete m_VAO;
	m_VBO->Delete();
	delete m_VBO;
	m_EBO->Delete();
	delete m_EBO;
}

void SkeletalMesh::Draw(
	Shader& shader,
	Camera& camera,
	glm::mat4 transform,
	glm::mat4 ntransform
) {
	shader.Activate();
	m_VAO->Bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	//numTextures uniform? tex array?
	//how to tell which tex to use?
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}

		textures[i].texUnit(shader, (type + num).c_str());
		textures[i].Bind();
	}

	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	camera.Matrix(shader, "camMatrix");

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "ntransform"), 1, GL_FALSE, glm::value_ptr(ntransform));

	//send transform matrix to meshes shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}


void SkeletalMesh::DrawShadow(
	Shader& shader,
	glm::mat4 transform
) {
	shader.Activate();
	m_VAO->Bind();

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

	//send transform matrix to meshes shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
