#include "renderer3d.h"

// Components
#include "components/3D/rigidbody3d.h"
#include "components/3D/transform3d.h"

void Renderer3D::init() {
	m_QuadRenderer = std::make_unique<QuadRenderer3D>();
	m_QuadRenderer->init();

	m_SphereRenderer = std::make_unique<SphereRenderer3D>();
	m_SphereRenderer->init();
	m_SphereRenderer->domeInit();
}

void Renderer3D::shutdown() {
	m_QuadRenderer->shutdown();
	m_SphereRenderer->shutdown();
}

void Renderer3D::beginFrame(PerspectiveCamera& camera) {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	m_ProjectionMatrix = camera.getViewProjectionMatrix();

	m_SphereRenderer->beginScene(m_ProjectionMatrix);
}

void Renderer3D::endFrame() {
	m_SphereRenderer->flush(m_ProjectionMatrix);
}

void Renderer3D::drawQuad(const glm::vec3& position, const glm::vec3& color, const glm::vec2& size) {
	m_QuadRenderer->drawQuad(position, color, size, m_ProjectionMatrix);
}

void Renderer3D::drawDome(const glm::vec3& position, const glm::vec3& color, float radius) {
	m_SphereRenderer->drawDome(position, color, radius, m_ProjectionMatrix);
}

void Renderer3D::drawSphere(const glm::vec3& position, const glm::vec3& color, float radius, const glm::quat& orientation) {
	m_SphereRenderer->drawSphere(position, color, radius, orientation);
}

void Renderer3D::drawSpheres(std::vector<std::unique_ptr<GameObject>>& spheres) {
	for (auto& obj : spheres) {
		auto* tf = obj->getComponent<Transform3D>();
		auto* rb = obj->getComponent<RigidBody3D>();
		if (rb) {
			m_SphereRenderer->drawSphere(rb->pos, glm::vec3(1.0f, 0.0f, 0.0f), 0.1f, rb->orientation);
		}
	}
}
