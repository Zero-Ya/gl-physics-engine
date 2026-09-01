#include "collision_simulation2d.h"

// Physics
#include "physics/2D/integrator2d.h"
#include "physics/2D/boundary_solver2d.h"
#include "physics/2D/collision_solver2d.h"

// Components
#include "components/2D/rigidbody2d.h"
#include "components/2D/transform2d.h"

CollisionSimulation2D::CollisionSimulation2D(SpatialGrid& spatialGrid)
    : m_SpatialGrid(spatialGrid)
{}

void CollisionSimulation2D::init(size_t initialCircleCount) {
    m_Scene = std::make_unique<Scene>();

    neighborCellIndices.reserve(9);
    clear();

    // Spawn circles with slight random velocities
    for (size_t i = 0; i < initialCircleCount; ++i) {
        float posX = ((rand() % 1000) / 1000.0f) * (m_WorldMax.x - m_WorldMin.x) + m_WorldMin.x;
        float posY = ((rand() % 1000) / 1000.0f) * (m_WorldMax.y - m_WorldMin.y) + m_WorldMin.y;
        float velX = ((rand() % 10) / 10.0f - 0.5f) * 1.0f;
        float velY = ((rand() % 10) / 10.0f - 0.5f) * 1.0f;

        spawnCircle(glm::vec2(3.5f, 2.5f), glm::vec2(velX, velY), 0.1f);
    }
}

void CollisionSimulation2D::update(float dt) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // 1. Euler integration
    integrateMotion(dt);

    // 2. Build grid
    buildSpatialGrid();

    // 3. Resolve circle-to-circle collisions
    resolveCollisions();

    // 4. Resolve world boundaries
    resolveBoundaryCollisions();

    auto endTime = std::chrono::high_resolution_clock::now();
    m_PhysicsTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
}

void CollisionSimulation2D::clear() {
    m_Scene->clearEntities();
    neighborCellIndices.clear();
}

void CollisionSimulation2D::spawnCircle(const glm::vec2& position, const glm::vec2& velocity, float radius) {
    m_Scene->createCircleEntity("Circle", position, velocity, radius);
}

void CollisionSimulation2D::integrateMotion(float dt) {
    for (auto& obj : m_Scene->getEntities()) {
        Integrator2D::integrate(obj.get(), dt, m_Gravity);
    }
}

void CollisionSimulation2D::resolveCollisions() {
    const auto& entities = m_Scene->getEntities();
    size_t entityCount = entities.size();

    for (size_t i = 0; i < entityCount; ++i) {
        auto* transformA = entities[i]->getComponent<Transform2D>();
        m_SpatialGrid.getNeighborCells(transformA->position, neighborCellIndices);

        for (size_t cellIdx : neighborCellIndices) {
            const auto& cell = m_SpatialGrid.getCell(cellIdx);

            for (size_t neighborIndex : cell.entityIndices) {
                if (i < neighborIndex) {
                    CollisionSolver2D::resolveCircleCollision(entities[i].get(), entities[neighborIndex].get());
                }
            }
        }
    }
}

void CollisionSimulation2D::resolveBoundaryCollisions() {
    for (auto& obj : m_Scene->getEntities()) {
        BoundarySolver2D::resolveCollision(obj.get(), m_WorldMin, m_WorldMax);
    }
}

void CollisionSimulation2D::buildSpatialGrid() {
    m_SpatialGrid.clear();
    for (size_t i = 0; i < m_Scene->getEntityCount(); ++i) {
        m_SpatialGrid.insert(i, m_Scene->getEntities()[i].get()->getComponent<Transform2D>()->position);
    }
}

void CollisionSimulation2D::updateSpatialGridAndWorldBounds(float virtualWidth, float virtualHeight, float cellSize) {
    m_SpatialGrid.resize(virtualWidth, virtualHeight, cellSize);

    m_WorldMin = glm::vec2(0.0f, 0.0f);
    m_WorldMax = glm::vec2(virtualWidth, virtualHeight);
}
