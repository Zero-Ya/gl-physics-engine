#include "integrator2d.h"
#include "scene/game_object.h"
#include "components/2D/transform2d.h"
#include "components/2D/rigidbody2d.h"

void Integrator2D::integrate(GameObject* obj, float dt, glm::vec2 gravity) {
	RigidBody2D* rb = obj->getComponent<RigidBody2D>();
	Transform2D* tf = obj->getComponent<Transform2D>();

	// Safeguard
	if (!rb || !tf || rb->mass <= 0.0f) return;

	// Euler integration?
	rb->force += gravity * rb->mass;

	glm::vec2 acceleration = rb->force * rb->inverseMass;

	rb->velocity += acceleration * dt;

	tf->position += rb->velocity * dt;

	rb->force = glm::vec2(0.0f, 0.0f);
}