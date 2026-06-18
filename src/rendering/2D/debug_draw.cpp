#include "debug_draw.h"
#include <glad/glad.h>
#include "rendering/utils/shader.h"

DebugDraw::DebugDraw(Shader& shader) : m_Shader(shader) {
	float vertices[] = {
		-0.5f, -0.5f, // Bottom-Left
		 0.5f, -0.5f, // Bottom-Right
		 0.5f,  0.5f, // Top-Right
		-0.5f,  0.5f  // Top-Left
	};

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

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
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glBindVertexArray(0);
}