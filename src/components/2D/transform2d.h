#pragma once
#include "scene/component.h"
#include <glm/glm.hpp>

struct Transform2D : public Component {
	glm::vec2 position;
	float rotation;
	float radius;

	Transform2D() = default;

	Transform2D(const glm::vec2& pos, float rotation, float radius)
		: position(pos), rotation(rotation), radius(radius) {
	}
};