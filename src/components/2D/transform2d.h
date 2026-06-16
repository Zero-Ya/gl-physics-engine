#pragma once
#include "scene/component.h"
#include <glm/glm.hpp>

struct Transform2D : public Component {
	glm::vec2 position{ 0.0f, 0.0f };
	float rotation{ 0.0f };

	Transform2D() = default;

	Transform2D(const glm::vec2& pos, float rotation = 0.0f)
		: position(pos), rotation(rotation) {
	}
};