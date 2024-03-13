#include "Mesh.h"
#include <glm/gtx/string_cast.hpp>

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, glm::mat4& transformation) {

	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
	Mesh::transform = transformation;

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
	glm::vec3& translation,
	glm::quat& rotation,
	glm::vec3& scale
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

		textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	// Take care of the camera Matrix  // ONLY needs to be sent once for shader!! (instancing)
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(90.0f, 0.1f, 1000.0f, shader, "camMatrix");

	// Transform the matrices to their correct form
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rot = glm::mat4_cast(rotation);
	glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));

	//send transform matrix to meshes shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}