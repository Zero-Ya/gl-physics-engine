#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <glm/glm.hpp>

#include "scene/scene.h"
#include "physics/2D/spatial_grid.h"

class CollisionSimulation2D {
public:
    CollisionSimulation2D(SpatialGrid& spatialGrid);
    ~CollisionSimulation2D() = default;

    void init(size_t initialCircleCount);
    void update(float dt);
    void clear();

    // Scene & Debug Management
    void spawnCircle(const glm::vec2& position, const glm::vec2& velocity, float radius);

    // Performance Tracking & Debug Overlays
    float getLastExecutionTimeMs() const { return m_PhysicsTimeMs; }
    size_t getEntityCount() const { return m_Scene2D->getEntityCount(); }
    std::vector<std::unique_ptr<GameObject>>& getCircles() const { return m_Scene2D->getEntities(); }
    const float& getPhysicsTimeMs() const { return m_PhysicsTimeMs; }

    // Physics Parameters
    void setGravity(const glm::vec2& gravity) { m_Gravity = gravity; }
    void setWorldBounds(const glm::vec2& minBounds, const glm::vec2& maxBounds) {
        m_WorldMin = minBounds;
        m_WorldMax = maxBounds;
    }

    void updateSpatialGridAndWorldBounds(float virtualWidth, float virtualHeight, float cellSize);

private:
    void integrateMotion(float dt);
    void resolveCollisions();
    void resolveBoundaryCollisions();

    void buildSpatialGrid();

private:
    std::unique_ptr<Scene2D> m_Scene2D;
    SpatialGrid& m_SpatialGrid;
    std::vector<size_t> neighborCellIndices;

    // World Constraints
    glm::vec2 m_Gravity { 0.0f, 0.0f };
    glm::vec2 m_WorldMin { 0.0f, 0.0f };
    glm::vec2 m_WorldMax { 10.0f,  10.0f };

    float m_PhysicsTimeMs = 0.0f;
};