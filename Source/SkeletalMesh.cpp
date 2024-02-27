#include "SkeletalMesh.h"
#include <glm/gtx/string_cast.hpp>

SkeletalMesh::SkeletalMesh(std::vector <SkeletalVertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, glm::mat4 transformation) {

	SkeletalMesh::vertices = vertices;
	SkeletalMesh::indices = indices;
	SkeletalMesh::textures = textures;
	SkeletalMesh::transform = transformation;

	VAO.Bind();

	SkeletalVBO  VBO(vertices);
	EBO  EBO(indices); // VBO & EBO bound during LinkAttrib links them to VAO!

	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(SkeletalVertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, normal));
	VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, texUV)); 

	//VAO.LinkAttrib(VBO, 3, 4, GL_FLOAT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, m_BoneIDs));
	VBO.Bind();
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, m_BoneIDs)); //AttribI function used instead!
	glEnableVertexAttribArray(3);
	VBO.Unbind();

	VAO.LinkAttrib(VBO, 4, 4, GL_FLOAT, sizeof(SkeletalVertex), (void*)offsetof(SkeletalVertex, m_Weights));

	VAO.Unbind();
	VBO.Unbind(); // VBO already unbinded by LinkAttrib()
	EBO.Unbind();
}


void SkeletalMesh::Draw(
	Shader& shader,
	Camera& camera,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
) {
	shader.Activate();
	VAO.Bind();

	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

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

		// IF we add more types of textures this function breaks!

		textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(90.0f, 0.1f, 1000.0f, shader, "camMatrix");

	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Transform the matrices to their correct form
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));

	//send transform matrix to meshes shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}