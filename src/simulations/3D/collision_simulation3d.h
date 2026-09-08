#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <glm/glm.hpp>

#include "scene/scene.h"

// Components
#include "components/3D/rigidbody3d.h"
#include "components/3D/transform3d.h"

class CollisionSimulation3D {
public:
	CollisionSimulation3D() = default;
	~CollisionSimulation3D() = default;

	void init(size_t initialSphereCount);
	void update(float dt);
	void clear();
    void resize(size_t newSphereCount);

    // Scene & Debug Management
    void spawnSphere(const glm::vec3& position, const glm::vec3& velocity, float radius);

    // Performance Tracking & Debug Overlays
    float getLastExecutionTimeMs() const { return m_PhysicsTimeMs; }
    size_t getEntityCount() const { return m_Scene3D->getEntityCount(); }
    std::vector<std::unique_ptr<GameObject>>& getSpheres() const { return m_Scene3D->getEntities(); }
    const float& getPhysicsTimeMs() const { return m_PhysicsTimeMs; }

    // Physics Parameters
    void setGravity(const glm::vec3& gravity) { m_Gravity = gravity; }

    void applyForces(glm::vec3 force);
    void applyTorques(glm::vec3 torque);

private:
    void integrate(float dt);

    void applyForce(RigidBody3D* rb, glm::vec3 force);
    void applyTorque(RigidBody3D* rb, glm::vec3 torque);

    void applyForceAtPoint(RigidBody3D* rb, glm::vec3 force, glm::vec3 localPoint);

    void detectCollisions(float dt);
    void resolveCollisionWithDome(RigidBody3D* rb, float domeRadius, float dt);
    void resolveCollision(RigidBody3D* rb1, RigidBody3D* rb2, float dt);

private:
    std::unique_ptr<Scene3D> m_Scene3D;

    // World Constraints
    glm::vec3 m_Gravity{ 0.0f, -9.81f, 0.0f };

    float friction = 0.5f;
    int solverSubsteps = 10; // Multiple substeps prevent high-speed tunneling

    float m_PhysicsTimeMs = 0.0f;
};
