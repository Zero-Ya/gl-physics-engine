#pragma once

#include <glm/glm.hpp>

#include "rendering/utils/shader.h"

class QuadRenderer3D {
public:
	QuadRenderer3D() = default;
	~QuadRenderer3D() = default;

	void init();
	void shutdown() const;

	void drawQuad(const glm::vec2& position, const glm::vec3& color, const glm::vec2& size, const glm::mat4& projection);

private:
	unsigned int m_VAO, m_VBO, m_EBO;
	std::unique_ptr<Shader> m_QuadShader;

};