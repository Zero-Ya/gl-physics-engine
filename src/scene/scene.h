#pragma once

#include "game_object.h"

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class Scene2D {
public:
	void createCircleEntity(const std::string& name = "CircleEntity",
							glm::vec2 pos = glm::vec2(0.0f, 0.0f),
							glm::vec2 vel = glm::vec2(0.0f, 0.0f),
							float radius = 0.0f);

	std::vector<std::unique_ptr<GameObject>>& getEntities() { return m_Entities; }
	size_t getEntityCount() const { return m_Entities.size(); }

	void clearEntities() { m_Entities.clear(); };

private:
	std::vector<std::unique_ptr<GameObject>> m_Entities;
	uint32_t entityId = 0;
};

class Scene3D {
public:
	GameObject* createSphereEntity(const std::string& name = "SphereEntity",
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f),
		float radius = 1.0f);

	std::vector<std::unique_ptr<GameObject>>& getEntities() { return m_Entities; }
	size_t getEntityCount() const { return m_Entities.size(); }

	void clearEntities() { m_Entities.clear(); };

private:
	std::vector<std::unique_ptr<GameObject>> m_Entities;
	uint32_t entityId = 0;
};