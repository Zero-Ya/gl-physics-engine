#pragma once
#include "scene/component.h"
#include <glm/glm.hpp>

struct Transform3D : public Component {
	glm::vec3 position;
	glm::vec3 rotation = glm::vec3(0.0f); // Euler angles in degrees (X, Y, Z)
	float radius; // Change to scale vec3 later?

	Transform3D() = default;

	Transform3D(const glm::vec3& pos, const glm::vec3& rotation, float radius)
		: position(pos), rotation(rotation), radius(radius) {
	}
};