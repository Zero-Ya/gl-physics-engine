#include "renderer2d.h"

// Components
#include "components/2D/rigidbody2d.h"
#include "components/2D/transform2d.h"

void Renderer2D::init() {
	m_CircleRenderer = std::make_unique<CircleRenderer>();
}

void Renderer2D::shutdown() {}


void Renderer2D::beginFrame() {
}

void Renderer2D::endFrame() {

}

void Renderer2D::submit(std::vector<std::unique_ptr<GameObject>>& circles) {
	// Not completely sure
	for (auto& obj : circles) {
		auto* tf = obj->getComponent<Transform2D>();
		if (tf) {
			m_CircleRenderer->drawCircle(tf->position, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), m_ProjectionMatrix, tf->radius);
		}
	}
}

void Renderer2D::setOrthographicProjection(float left, float right, float bottom, float top) {
	m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}
