#include "circle_renderer2d.h"
#include <glad/glad.h>

void CircleRenderer::init() {
	m_CircleBatch.reserve(MAX_CIRCLES);

	// Create the shader
	m_CircleShader = std::make_unique<Shader>("assets/shaders/2D/circle_renderer2d.vert", "assets/shaders/2D/circle_renderer2d.frag");

	float quadVertices[] = {
		// Positions	Tex coords
		-0.5f, -0.5f,	1.0f, 1.0f, // Bottom-Left
		 0.5f, -0.5f,	1.0f, 0.0f, // Bottom-Right
		 0.5f,  0.5f,	0.0f, 0.0f, // Top-Right
		-0.5f,  0.5f,	0.0f, 1.0f  // Top-Left
	};

	unsigned int quadIndices[] = {
		0, 1, 3, // First triangle
		1, 2, 3  // Second triangle
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Static quad VBO and EBO
	glGenBuffers(1, &m_QuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	// Tex coords attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	/////////////////////////////////////////////////////////////////////////////////////////////////

	// Dynamic instance VBO
	glGenBuffers(1, &m_InstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, MAX_CIRCLES * sizeof(CircleData), nullptr, GL_DYNAMIC_DRAW);

	// Instanced attribute setup
	uint32_t stride = sizeof(CircleData);

	// Attribute 2: Position
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(CircleData, position));
	glVertexAttribDivisor(2, 1); // Step once per instance

	// Attribute 3: Color
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(CircleData, color));
	glVertexAttribDivisor(3, 1);

	// Attribute 4: Radius
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(CircleData, radius));
	glVertexAttribDivisor(4, 1);

	glBindVertexArray(0);
}

void CircleRenderer::shutdown() {
	if (m_InstanceVBO) glDeleteBuffers(1, &m_InstanceVBO);
	if (m_QuadVBO)     glDeleteBuffers(1, &m_QuadVBO);
	if (m_EBO)         glDeleteBuffers(1, &m_EBO);
	if (m_VAO)         glDeleteVertexArrays(1, &m_VAO);
	m_CircleBatch.clear();
}

void CircleRenderer::drawCircle(const glm::vec2& position, const glm::vec3& color, float radius) {
	if (m_CircleBatch.size() >= MAX_CIRCLES) {
		std::cerr << "[CircleSubRenderer] Warning: Max circle batch capacity reached!\n";
		return;
	}

	m_CircleBatch.push_back({ position, color, radius });
}

void CircleRenderer::flush(const glm::mat4& projection) {
	if (m_CircleBatch.empty()) return;

	// Enable Alpha Blending for smooth SDF edges
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_CircleShader->use();
	m_CircleShader->setMat4("u_Projection", projection);

	glBindVertexArray(m_VAO);

	// Upload CPU instance array directly to Instance VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	GLsizeiptr dataSize = m_CircleBatch.size() * sizeof(CircleData);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_CircleBatch.data());

	// Issue ONE instanced draw call for all accumulated circles
	GLsizei instanceCount = static_cast<GLsizei>(m_CircleBatch.size());
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, instanceCount);

	glBindVertexArray(0);
	m_CircleBatch.clear();
}