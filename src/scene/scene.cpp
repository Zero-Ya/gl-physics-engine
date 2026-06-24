#include "scene.h"
#include "components/2D/rigidbody2d.h"
#include "components/2D/transform2d.h"

GameObject* Scene::createEntity(const std::string& name, glm::vec2 pos, glm::vec2 vel) {
	auto newObj = std::make_unique<GameObject>(entityId++, name);

	newObj->addComponent<Transform2D>(pos);
	auto rb = newObj->addComponent<RigidBody2D>(1.0f);
	rb->velocity = vel * 5.0f;

	m_Entities.push_back(std::move(newObj));

	return newObj.get();
}