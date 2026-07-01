#pragma once

#include "game_object.h"

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>

class Scene {
public:
	GameObject* createEntity(const std::string& name = "EmptyEntity",
							 glm::vec2 pos = glm::vec2(0.0f, 0.0f),
							 glm::vec2 vel = glm::vec2(0.0f, 0.0f),
							 float radius = 0.5f);

	std::vector<std::unique_ptr<GameObject>>& getEntities() { return m_Entities; }
	size_t getEntityCount() const { return m_Entities.size(); }

private:
	std::vector<std::unique_ptr<GameObject>> m_Entities;
	uint32_t entityId = 0;
};