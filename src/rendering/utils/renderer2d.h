#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene/scene.h"
#include "shader.h"
#include "rendering/2D/circle_renderer.h"

struct Vertex2D {
	glm::vec2 position;
	glm::vec4 color;
};

class Renderer2D {
public:
	Renderer2D() = default;
	~Renderer2D() = default;

	void init();
	void shutdown();

	void beginFrame();
	void endFrame();
	void submit(std::vector<std::unique_ptr<GameObject>>& circles);

	void setOrthographicProjection(float left, float right, float bottom, float top);

private:
	std::unique_ptr<CircleRenderer> m_CircleRenderer;

	glm::vec2 m_CameraPosition { 0.0f, 1.0f };
	glm::mat4 m_ProjectionMatrix { 1.0f };
	glm::mat4 m_ViewMatrix { 1.0f };
	float m_Zoom { 1.0f };
};
