#include "Mesh.h"
#include <glm/gtx/string_cast.hpp>

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, glm::mat4& model) {

	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
	Mesh::model = model;

	m_VAO = new VAO();

	m_VAO->Bind();

	m_VBO = new VBO(vertices);
	m_EBO = new EBO(indices);

	m_VAO->LinkAttrib(*m_VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // pos
	m_VAO->LinkAttrib(*m_VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float))); // normal
	m_VAO->LinkAttrib(*m_VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float))); // texUV

	m_VAO->Unbind();
	m_VBO->Unbind(); // VBO already unbinded by LinkAttrib()
	m_EBO->Unbind();
}

Mesh::~Mesh() {
	m_VAO->Delete();
	delete m_VAO;
	m_VBO->Delete();
	delete m_VBO;
	m_EBO->Delete();
	delete m_EBO;
}

void Mesh::Draw(
	Shader& shader,
	Camera& camera,
	glm::mat4 transform,
	glm::mat4 ntransform
) {
	shader.Activate();
	m_VAO->Bind();
	
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

		textures[i].texUnit(shader, (type + num).c_str());
		textures[i].Bind();
	}

	/* SEND THESE TO ALL SHADERS ONCE BEFORE DRAWING */
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(Globals::get().lightSpaceMatrix));

	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D, Globals::get().depthMap);
	glUniform1i(glGetUniformLocation(shader.ID, "shadowMap"), 6);

	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Globals::get().depthCubeMap);
	glUniform1i(glGetUniformLocation(shader.ID, "shadowCubeMap"), 7);

	glUniform1f(glGetUniformLocation(shader.ID, "far_plane"), Globals::get().far_plane);

	// Take care of the camera Matrix  // ONLY needs to be sent once for shader!! (instancing)
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	camera.Matrix(shader, "camMatrix");
	/* SEND THESE TO ALL SHADERS ONCE BEFORE DRAWING */

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "ntransform"), 1, GL_FALSE, glm::value_ptr(ntransform));

	//send transform matrix to meshes shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::DrawShadow(
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