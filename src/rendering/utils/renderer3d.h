#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "rendering/3D/quad_renderer3d.h"

#include "perspective_camera.h"

class Renderer3D {
public:
	Renderer3D() = default;
	~Renderer3D() = default;

	void init();
	void shutdown();

	void beginFrame(PerspectiveCamera& camera);
	void endFrame();
	void submit();

	void drawQuad(const glm::vec2& position, const glm::vec3& color, const glm::vec2& size);

private:
	std::unique_ptr<QuadRenderer3D> m_QuadRenderer;

	glm::vec2 m_CameraPosition{ 0.0f, 1.0f };
	glm::mat4 m_ProjectionMatrix{ 1.0f };
	glm::mat4 m_ViewMatrix{ 1.0f };
	float m_Zoom{ 1.0f };
};
