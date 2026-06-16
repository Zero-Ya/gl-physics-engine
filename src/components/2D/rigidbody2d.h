#pragma once
#include "scene/component.h"
#include <glm/glm.hpp>

struct RigidBody2D : public Component {
	glm::vec2 velocity{ 0.0f, 0.0f };
	glm::vec2 force{ 0.0f, 0.0f };

	float mass{ 1.0f };
	float inverseMass{ 1.0f };

	RigidBody2D() = default;
	RigidBody2D(float mass) { setMass(mass); }

	void setMass(float m) {
		mass = m;
		if (mass > 0.0f) {
			inverseMass = 1.0f / mass;
		}
		else {
			inverseMass = 0.0f;
		}
	}
};
