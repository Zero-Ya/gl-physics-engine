#include "debug_draw.h"
#include <glad/glad.h>
#include "rendering/utils/shader.h"

DebugDraw::DebugDraw(Shader& shader) : m_Shader(shader) {
	float vertices[] = {
		// Positions	Tex coords
		-0.5f, -0.5f,	1.0f, 1.0f, // Bottom-Left
		 0.5f, -0.5f,	1.0f, 0.0f, // Bottom-Right
		 0.5f,  0.5f,	0.0f, 0.0f, // Top-Right
		-0.5f,  0.5f,	0.0f, 1.0f  // Top-Left
	};

	unsigned int indices[] = {
		0, 1, 3, // First triangle
		1, 2, 3  // Second triangle
	};

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	// Tex coords attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void DebugDraw::drawBox(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, const glm::mat4& projection)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(size, 1.0f));

	m_Shader.use();
	m_Shader.setMat4("u_Model", model);
	m_Shader.setMat4("u_Projection", projection);
	m_Shader.setVec3("u_Color", color);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}