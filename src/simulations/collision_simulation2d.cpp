#include "collision_simulation2d.h"

// Physics
#include "physics/2D/integrator2d.h"
#include "physics/2D/boundary_solver2d.h"
#include "physics/2D/collision_solver2d.h"

// Components
#include "components/2D/rigidbody2d.h"
#include "components/2D/transform2d.h"

void CollisionSimulation2D::init(size_t initialCircleCount, float aspectedWidth, float aspectedHeight) {
    m_Scene = std::make_unique<Scene>();

    // Init spatial grid
    m_SpatialGrid =  std::make_unique<SpatialGrid>(aspectedWidth * 2.0f, aspectedHeight * 2.0f, 0.5f);
    neighborCellIndices.reserve(9);

    clear();

    // Example initialization: Spawn circles with slight random velocities
    for (size_t i = 0; i < initialCircleCount; ++i) {
        float posX = ((rand() % 1000) / 1000.0f) * (m_WorldMax.x - m_WorldMin.x) + m_WorldMin.x;
        float posY = ((rand() % 1000) / 1000.0f) * (m_WorldMax.y - m_WorldMin.y) + m_WorldMin.y;
        float velX = ((rand() % 10) / 10.0f - 0.5f) * 1.0f;
        float velY = ((rand() % 10) / 10.0f - 0.5f) * 1.0f;

        spawnCircle(glm::vec2(0.0f, 0.0f), glm::vec2(velX, velY), 0.1f);
    }
}

void CollisionSimulation2D::update(float dt, float aspectedWidth, float aspectedHeight) {
    auto startTime = std::chrono::high_resolution_clock::now();

    buildSpatialGrid(aspectedWidth, aspectedHeight);

    // 1. Euler integration
    integrateMotion(dt);

    // 2. Resolve circle-to-circle collisions
    resolveCollisions(aspectedWidth, aspectedHeight);

    // 3. Resolve world boundaries
    resolveBoundaryCollisions();

    auto endTime = std::chrono::high_resolution_clock::now();
    m_PhysicsTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
}

void CollisionSimulation2D::clear() {
    // Clear scene
}

void CollisionSimulation2D::spawnCircle(const glm::vec2& position, const glm::vec2& velocity, float radius) {
    m_Scene->createCircleEntity("Circle", position, velocity, radius);
}

void CollisionSimulation2D::integrateMotion(float dt) {
    for (auto& obj : m_Scene->getEntities()) {
        Integrator2D::integrate(obj.get(), dt, m_Gravity);
    }
}

void CollisionSimulation2D::resolveCollisions(float aspectedWidth, float aspectedHeight) {
    for (size_t i = 0; i < m_Scene->getEntityCount(); ++i) {
        m_SpatialGrid->getNeighborCells(m_Scene->getEntities()[i].get()->getComponent<Transform2D>()->position, neighborCellIndices, aspectedWidth, aspectedHeight);

        for (size_t cellIdx : neighborCellIndices) {
            const auto& cell = m_SpatialGrid->getCell(cellIdx);

            for (size_t neighborIndex : cell.entityIndices) {
                // Double-Check Protection: 
                // 1. Don't check an entity against itself
                // 2. Only resolve if i < neighborIndex to enforce one check per pair
                if (i >= neighborIndex) continue;
                CollisionSolver2D::resolveCircleCollision(m_Scene->getEntities()[i].get(), m_Scene->getEntities()[neighborIndex].get());
            }
        }
    }
}

void CollisionSimulation2D::resolveBoundaryCollisions() {
    for (auto& obj : m_Scene->getEntities()) {
        BoundarySolver2D::resolveCollision(obj.get(), m_WorldMin, m_WorldMax);
    }
}

void CollisionSimulation2D::buildSpatialGrid(float aspectedWidth, float aspectedHeight) {
    // Spatial grid build ??
    m_SpatialGrid->clear();
    for (size_t i = 0; i < m_Scene->getEntityCount(); ++i) {
        m_SpatialGrid->insert(i, m_Scene->getEntities()[i].get()->getComponent<Transform2D>()->position, aspectedWidth, aspectedHeight); // Bruh
    }
}

void CollisionSimulation2D::updateSpatialGridAndWorldBounds(float aspectedWidth, float aspectedHeight) {
    m_SpatialGrid->resize(aspectedWidth * 2.0f, aspectedHeight * 2.0f);

    m_WorldMin = glm::vec2(-aspectedWidth, -aspectedHeight);
    m_WorldMax = glm::vec2(aspectedWidth, aspectedHeight);
}
