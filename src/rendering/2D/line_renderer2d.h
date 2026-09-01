#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "rendering/utils/shader.h"

struct LineVertex {
	glm::vec2 position;
	glm::vec3 color;
};

class LineRenderer {
public:
	LineRenderer() = default;
	~LineRenderer() = default;

	void init();
	void shutdown();

	void drawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec3& color);
	void flush(const glm::mat4& projection);

private:
	std::vector<LineVertex> m_LineBatch;

	// Buffer caps
	static constexpr size_t MAX_LINES = 10000;
	static constexpr size_t MAX_VERTICES = MAX_LINES * 2;

	unsigned int m_VBO, m_VAO;
	std::unique_ptr<Shader> m_LineShader;
};