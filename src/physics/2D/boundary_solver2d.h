#pragma once

#include <glm/glm.hpp>

class GameObject;

class BoundarySolver2D {
public:
	static void resolveCollision(GameObject* obj, const glm::vec2& minBounds, const glm::vec2& maxBounds);
};