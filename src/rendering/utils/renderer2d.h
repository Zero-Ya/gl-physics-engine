#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene/scene.h"
#include "shader.h"
#include "rendering/2D/circle_renderer2d.h"
#include "rendering/2D/line_renderer2d.h"

class Renderer2D {
public:
	Renderer2D() = default;
	~Renderer2D() = default;

	void init();
	void shutdown();

	void beginFrame();
	void endFrame();
	void submit();

	void drawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec3& color);
	void drawCircle(const glm::vec2& position, const glm::vec3& color, float radius);
	void drawCircles(std::vector<std::unique_ptr<GameObject>>& circles);

	void setOrthographicProjection(float left, float right, float bottom, float top);

	std::unique_ptr<LineRenderer>& getLineRenderer() { return m_LineRenderer; };

private:
	std::unique_ptr<LineRenderer> m_LineRenderer;
	std::unique_ptr<CircleRenderer> m_CircleRenderer;

	glm::vec2 m_CameraPosition { 0.0f, 1.0f };
	glm::mat4 m_ProjectionMatrix { 1.0f };
	glm::mat4 m_ViewMatrix { 1.0f };
	float m_Zoom { 1.0f };
};
