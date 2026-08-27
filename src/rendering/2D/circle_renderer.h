#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "rendering/utils/shader.h"

struct CircleData {
    glm::vec2 position;
    glm::vec3 color;
    float radius;
};

class CircleRenderer {
public:
    CircleRenderer() = default;
    ~CircleRenderer() = default;

    void init();
    void shutdown();

	void drawCircle(const glm::vec2& position, const glm::vec3& color, float radius);
    void flush(const glm::mat4& projection);

private:
    std::vector<CircleData> m_CircleBatch;

    static constexpr size_t MAX_CIRCLES = 10000;

	unsigned int m_VAO, m_EBO;
    unsigned int m_QuadVBO = 0;
    unsigned int m_InstanceVBO = 0;
	std::unique_ptr<Shader> m_CircleShader;
};