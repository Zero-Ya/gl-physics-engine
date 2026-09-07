#pragma once
#include "scene/component.h"

#include <glm/glm.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/quaternion.hpp>

struct RigidBody3D : public Component {
	glm::vec3 prevPos = glm::vec3(0.0f);
	glm::vec3 pos = glm::vec3(0.0f);
	glm::vec3 vel = glm::vec3(0.0f);
	glm::vec3 acc = glm::vec3(0.0f);

	glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 angVel = glm::vec3(0.0f);
	glm::vec3 angAcc = glm::vec3(0.0f);

	float mass{ 1.0f };
	float inverseMass{ 1.0f };
	float radius{ 1.0f };

	glm::mat3 inertia = glm::mat3(1.0f);
	glm::mat3 inverseInertia = glm::mat3(1.0f);

	RigidBody3D() = default;
	RigidBody3D(float mass, glm::vec3 pos, glm::vec3 vel, float radius) : prevPos(pos), pos(pos), vel(vel), radius(radius)
	{ setMass(mass); }

	void setMass(float m) {
		mass = m;
		inverseMass = (m > 0.0f) ? 1.0f / m : 0.0f;
	}

	void calculateInertia() {
		float I = 2.0f / 5.0f * mass * mass * radius;
		inertia = glm::mat3(
			I, 0.0f, 0.0f,
			0.0f, I, 0.0f,
			0.0f, 0.0f, I
		);
		inverseInertia = glm::inverse(inertia);
	}

};
