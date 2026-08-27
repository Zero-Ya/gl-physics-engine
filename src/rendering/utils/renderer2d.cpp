#include "renderer2d.h"

// Components
#include "components/2D/rigidbody2d.h"
#include "components/2D/transform2d.h"

void Renderer2D::init() {
	m_LineRenderer = std::make_unique<LineRenderer>();
	m_LineRenderer->init();
	m_CircleRenderer = std::make_unique<CircleRenderer>();
	m_CircleRenderer->init();

	//setOrthographicProjection();
}

void Renderer2D::shutdown() {
	m_LineRenderer->shutdown();
	m_CircleRenderer->shutdown();
}


void Renderer2D::beginFrame() {
}

void Renderer2D::endFrame() {

}

void Renderer2D::submit() {
	m_LineRenderer->flush(m_ProjectionMatrix);
	m_CircleRenderer->flush(m_ProjectionMatrix);
}

void Renderer2D::drawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec3& color) {
	m_LineRenderer->drawLine(p1, p2, color);
}

void Renderer2D::drawCircle(const glm::vec2& position, const glm::vec3& color, float radius) {
	m_CircleRenderer->drawCircle(position, color, radius);
}

void Renderer2D::drawCircles(std::vector<std::unique_ptr<GameObject>>& circles) {
	// Not completely sure
	for (auto& obj : circles) {
		auto* tf = obj->getComponent<Transform2D>();
		if (tf) {
			m_CircleRenderer->drawCircle(tf->position, glm::vec3(0.0f, 1.0f, 0.0f), tf->radius);
		}
	}
}

void Renderer2D::setOrthographicProjection(float left, float right, float bottom, float top) {
	m_ProjectionMatrix = glm::ortho(0.0f, right, 0.0f, top, -1.0f, 1.0f);
}
