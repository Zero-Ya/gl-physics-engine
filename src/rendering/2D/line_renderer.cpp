#include "line_renderer.h"

#include <glad/glad.h>
#include <iostream>

void LineRenderer::init() {
    m_LineBatch.reserve(MAX_VERTICES);

    // Create the shader
    m_LineShader = std::make_unique<Shader>("assets/shaders/line_renderer2d.vert", "assets/shaders/line_renderer2d.frag");

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // Allocate orphan GPU memory upfront using GL_DYNAMIC_DRAW
    size_t maxBufferSize = MAX_VERTICES * sizeof(LineVertex);
    glBufferData(GL_ARRAY_BUFFER, maxBufferSize, nullptr, GL_DYNAMIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);

    // Color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void LineRenderer::shutdown() {
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    m_LineBatch.clear();
}

void LineRenderer::drawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec3& color) {
    if (m_LineBatch.size() >= MAX_VERTICES) {
        std::cerr << "[LineSubRenderer] Warning: Line batch capacity exceeded!\n";
        return;
    }

    m_LineBatch.push_back({ p1, color });
    m_LineBatch.push_back({ p2, color });
}

void LineRenderer::flush(const glm::mat4& projection) {
    if (m_LineBatch.empty()) return;

    m_LineShader->use();
    m_LineShader->setMat4("u_Projection", projection);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    size_t dataSize = m_LineBatch.size() * sizeof(LineVertex);
    glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_LineBatch.data());

    size_t vertexCount = static_cast<GLsizei>(m_LineBatch.size());
    glDrawArrays(GL_LINES, 0, vertexCount);

    glBindVertexArray(0);
    m_LineBatch.clear();
}