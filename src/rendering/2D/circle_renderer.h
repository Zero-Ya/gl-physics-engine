#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "rendering/utils/shader.h"

class CircleRenderer {
public:
	CircleRenderer();

	void drawCircle(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, const glm::mat4& projection, float radius);

private:
	unsigned int m_VBO, m_VAO, m_EBO;
	std::unique_ptr<Shader> m_CircleShader;
};