#pragma once

#include <glm/glm.hpp>

class GameObject;

class Integrator2D {
public:
	static void integrate(GameObject& obj, float dt, glm::vec2 gravity);

};