#pragma once

#include <glm/gtc/matrix_transform.hpp>

class Shader;

class DebugDraw {
public:
	DebugDraw(Shader& shader);

	void drawBox(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, const glm::mat4& projection);

private:
	unsigned int m_VBO, m_VAO;
	Shader& m_Shader;
};