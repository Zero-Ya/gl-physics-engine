#pragma once

#include <glm/glm.hpp>

class GameObject;

class CollisionSolver2D {
public:
	static void resolveCircleCollision(GameObject* objA, GameObject* objB);
};