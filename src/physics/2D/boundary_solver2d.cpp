#include "boundary_solver2d.h"
#include "scene/game_object.h"
#include "components/2D/transform2d.h"
#include "components/2D/rigidbody2d.h"

void BoundarySolver2D::resolveCollision(GameObject* obj, const glm::vec2& minBounds, const glm::vec2& maxBounds) {
	RigidBody2D* rb = obj->getComponent<RigidBody2D>();
	Transform2D* tf = obj->getComponent<Transform2D>();
	
	if (!tf || !rb) return;

	float radius = 0.5f;
	float restitution = 1.0f; // Bounciness

    // X Axis collision
    // Right wall
    if (tf->position.x + radius > maxBounds.x) {
        tf->position.x = maxBounds.x - radius;
        rb->velocity.x = -rb->velocity.x * restitution;
    }
    // Left wall
    else if (tf->position.x - radius < minBounds.x) {
        tf->position.x = minBounds.x + radius;
        rb->velocity.x = -rb->velocity.x * restitution;
    }

    // Y Axis collision
    // Ceiling
    if (tf->position.y + radius > maxBounds.y) {
        tf->position.y = maxBounds.y - radius;
        rb->velocity.y = -rb->velocity.y * restitution;
    }
    // Floor
    else if (tf->position.y - radius < minBounds.y) {
        tf->position.y = minBounds.y + radius;
        rb->velocity.y = -rb->velocity.y * restitution;
    }
}