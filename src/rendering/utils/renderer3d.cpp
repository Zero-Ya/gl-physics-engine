#include "renderer3d.h"



void Renderer3D::init() {
	m_QuadRenderer = std::make_unique<QuadRenderer3D>();
	m_QuadRenderer->init();

}

void Renderer3D::shutdown() {
	m_QuadRenderer->shutdown();
}


void Renderer3D::beginFrame(PerspectiveCamera& camera) {
	m_ProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer3D::endFrame() {

}

void Renderer3D::submit() {

}

void Renderer3D::drawQuad(const glm::vec2& position, const glm::vec3& color, const glm::vec2& size) {
	m_QuadRenderer->drawQuad(position, color, size, m_ProjectionMatrix);
}