#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <glm/glm.hpp>

#include "scene/scene.h"
#include "physics/2D/spatial_grid.h"

struct CircleEntity {
    glm::vec2 position;
    glm::vec2 velocity;
    float radius;
    float mass;
    glm::vec4 color;
};

class CollisionSimulation2D {
public:
    CollisionSimulation2D() = default;
    ~CollisionSimulation2D() = default;

    void init(size_t initialCircleCount, float aspectedWidth, float aspectedHeight);
    void update(float dt, float aspectedWidth, float aspectedHeight);
    void clear();

    // Scene & Debug Management
    void spawnCircle(const glm::vec2& position, const glm::vec2& velocity, float radius);

    // Performance Tracking & Debug Overlays
    float getLastExecutionTimeMs() const { return m_PhysicsTimeMs; }
    size_t getEntityCount() const { return m_Scene->getEntityCount(); }
    std::vector<std::unique_ptr<GameObject>>& getCircles() const { return m_Scene->getEntities(); }
    const float& getPhysicsTimeMs() const { return m_PhysicsTimeMs; }

    // Physics Parameters
    void setGravity(const glm::vec2& gravity) { m_Gravity = gravity; }
    void setWorldBounds(const glm::vec2& minBounds, const glm::vec2& maxBounds) {
        m_WorldMin = minBounds;
        m_WorldMax = maxBounds;
    }

    void updateSpatialGridAndWorldBounds(float aspectedWidth, float aspectedHeight);

    std::unique_ptr<SpatialGrid>& getSpatialGrid() { return m_SpatialGrid; }

private:
    void integrateMotion(float dt);
    void resolveCollisions(float aspectedWidth, float aspectedHeight);
    void resolveBoundaryCollisions();

    void buildSpatialGrid(float aspectedWidth, float aspectedHeight);

private:
    std::unique_ptr<Scene> m_Scene;
    std::unique_ptr<SpatialGrid> m_SpatialGrid;
    std::vector<size_t> neighborCellIndices;

    // World Constraints
    glm::vec2 m_Gravity { 0.0f, 0.0f }; // Zero gravity
    //glm::vec2 m_Gravity{ 0.0f, -9.81f };
    glm::vec2 m_WorldMin { -10.0f, -10.0f };
    glm::vec2 m_WorldMax { 10.0f,  10.0f };

    float m_PhysicsTimeMs = 0.0f;

    bool showSpatialGrid = false;
};