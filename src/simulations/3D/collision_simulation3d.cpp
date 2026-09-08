#include "collision_simulation3d.h"

#include <algorithm>

#include <glfw/glfw3.h>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/quaternion.hpp>

#include "core/input.h"

glm::vec3 getLocalPos(RigidBody3D* rb, glm::vec3 worldPos);
glm::vec3 getWorldPos(RigidBody3D* rb, glm::vec3 localPos);
glm::vec3 getVelAtPoint(RigidBody3D* rb, glm::vec3 localPoint);

void CollisionSimulation3D::init(size_t initialSphereCount) {
    m_Scene3D = std::make_unique<Scene3D>();

    resize(initialSphereCount);
}

void CollisionSimulation3D::update(float dt) {
    auto startTime = std::chrono::high_resolution_clock::now();

    //dt = std::clamp(dt, 0.0001f, 0.0166f);

    float subDt = dt / static_cast<float>(solverSubsteps);
    for (uint32_t step = 0; step < solverSubsteps; ++step) {

        // Gravity
        applyForces(m_Gravity);

        // Collisions
        detectCollisions(subDt);

        integrate(subDt);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    m_PhysicsTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
}

void CollisionSimulation3D::integrate(float dt) {
    for (auto& entity : m_Scene3D->getEntities()) {
        auto* rb = entity->getComponent<RigidBody3D>();

        // Floor collision
        if (rb->pos.y < -1) {
            rb->pos.y = -1.0f;
            rb->vel.y = -rb->vel.y * 0.85f;
        }

        // Verlet integration
        //rb->prevPos = rb->pos;
        //// Derived velocity: displacement * friction damping
        //glm::vec3 velocity = (rb->pos - rb->prevPos) * 0.85f;
        //rb->pos += velocity + rb->acc * (dt * dt);


        rb->pos += rb->vel * dt;
        rb->vel += rb->acc * dt;

        rb->angVel += rb->angAcc * dt;

        glm::quat dtOrientation = 0.5f * rb->orientation * dt * glm::quat(0.0f, rb->angVel.x, rb->angVel.y, rb->angVel.z);
        rb->orientation = glm::normalize(rb->orientation + dtOrientation);

        rb->acc = glm::vec3(0.0f);
        rb->angAcc = glm::vec3(0.0f);
    }
}

void CollisionSimulation3D::applyForce(RigidBody3D* rb, glm::vec3 force) {
    rb->acc += force * rb->inverseMass;
}

void CollisionSimulation3D::applyTorque(RigidBody3D* rb, glm::vec3 torque) {
    rb->angAcc += rb->inverseInertia * torque;
}

void CollisionSimulation3D::applyForces(glm::vec3 force) {
    for (auto& entity : m_Scene3D->getEntities()) {
        auto* rb = entity->getComponent<RigidBody3D>();

        rb->acc += force * rb->inverseMass;
    }
}

void CollisionSimulation3D::applyTorques(glm::vec3 torque) {
    for (auto& entity : m_Scene3D->getEntities()) {
        auto* rb = entity->getComponent<RigidBody3D>();

        rb->angAcc += rb->inverseInertia * torque;
    }
}

void CollisionSimulation3D::applyForceAtPoint(RigidBody3D* rb, glm::vec3 force, glm::vec3 localPoint) {
    glm::vec3 localForce = glm::conjugate(rb->orientation) * force;
    glm::vec3 torque = glm::cross(localPoint, localForce);
    applyForce(rb, force);
    applyTorque(rb, torque);
}

void CollisionSimulation3D::detectCollisions(float dt) {
    float domeRadius = 2.0f;
    glm::vec3 domeCenter = glm::vec3(0.0f, 2.0f, 0.0f);

    for (int i =  0; i < m_Scene3D->getEntityCount(); i++) {
        auto& entity = m_Scene3D->getEntities()[i];
        auto* rb = entity->getComponent<RigidBody3D>();

        float collideDistance = (glm::length(rb->pos - domeCenter) + rb->radius);
        if ((glm::length(rb->pos - domeCenter) + rb->radius) > domeRadius) {
            resolveCollisionWithDome(rb, domeRadius, dt);
        }

        for (int j = i + 1; j < m_Scene3D->getEntityCount(); j++) {
            auto& otherEntity = m_Scene3D->getEntities()[j];
            auto* rbO = otherEntity->getComponent<RigidBody3D>();
            if (glm::distance(rb->pos, rbO->pos) < rbO->radius + rb->radius) {
                resolveCollision(rb, rbO, dt);
            }

        }
    }
}

void CollisionSimulation3D::resolveCollisionWithDome(RigidBody3D* rb, float domeRadius, float dt) {
    if (!rb || dt <= 0.00001f) return;

    glm::vec3 domeCenter = glm::vec3(0.0f, 2.0f, 0.0f);
    glm::vec3 relativePos = rb->pos - domeCenter;
    float distFromCenter = glm::length(relativePos);

    // Guard against division by zero at center
    if (distFromCenter <= 0.00001f) return;

    // Dome boundary collision check
    float penetration = (distFromCenter + rb->radius) - domeRadius;
    if (penetration <= 0.0f) return; // Inside dome, no collision

    // Inward normal pointing from dome boundary toward center (pushing back inside)
    glm::vec3 normal = -relativePos / distFromCenter;

    // Contact point on the sphere surface touching the dome
    glm::vec3 contactPoint = rb->pos - normal * rb->radius;
    glm::vec3 localContactPoint = getLocalPos(rb, contactPoint);

    // Surface velocity at contact point
    glm::vec3 relativeVel = getVelAtPoint(rb, localContactPoint);
    float normalVelMag = glm::dot(relativeVel, normal); // Negative if moving outward/into wall
    glm::vec3 normalVel = normal * normalVelMag;
    glm::vec3 tangentialVel = relativeVel - normalVel;

    // Cap maximum penetration to 10% of dome radius to prevent extreme force spikes
    float cappedPenetration = std::min(penetration, domeRadius * 0.10f);
    glm::vec3 normalDisplacement = normal * cappedPenetration;

    float normalStiffness = 10000.0f;
    float normalDampening = 100.0f;

    // Normal spring force
    glm::vec3 normalSpringForce = normalDisplacement * normalStiffness;

    // Damping force (Clamped)
    glm::vec3 normalDampingForce = glm::vec3(0.0f);
    if (normalVelMag < 0.0f) { // Only damp when sphere is moving into the dome wall
        float approachSpeed = -normalVelMag;
        float maxStoppingForce = (rb->mass * approachSpeed) / dt;
        float rawDampingForce = rb->mass * normalDampening * approachSpeed;

        float clampedDamping = std::min(rawDampingForce, maxStoppingForce);
        normalDampingForce = normal * clampedDamping;
    }

    glm::vec3 normalForce = normalSpringForce + normalDampingForce;

    // Tangential shear & friction
    glm::vec3 tangentialDisplacement = dt * tangentialVel;
    float tangentialStiffness = 200.0f;
    float tangentialDampening = 10.0f;

    glm::vec3 tangentialForce = (tangentialDisplacement * tangentialStiffness)
        - (rb->mass * tangentialDampening * tangentialVel);

    // Coulomb friction limit using normal spring force component
    float maxFrictionForce = friction * glm::length(normalSpringForce);
    float tangentialForceMag = glm::length(tangentialForce);

    if (tangentialForceMag > maxFrictionForce && tangentialForceMag > 0.00001f) {
        tangentialForce = (tangentialForce / tangentialForceMag) * maxFrictionForce;
    }

    // Apply forces
    applyForce(rb, normalForce);
    applyForceAtPoint(rb, tangentialForce, localContactPoint);
}

void CollisionSimulation3D::resolveCollision(RigidBody3D* rb1, RigidBody3D* rb2, float dt) {
    if (!rb1 || !rb2 || dt <= 0.00001f) return;

    // 1. Check relative distance and overlap
    glm::vec3 posDt = rb2->pos - rb1->pos;
    float dist = glm::length(posDt);
    float minDist = rb1->radius + rb2->radius;

    // Exit early if not colliding or if centers are identical (prevent division by zero)
    if (dist >= minDist || dist <= 0.00001f) return;

    // Normal point from rb1 to rb2
    glm::vec3 normal = posDt / dist;

    // Contact points
    glm::vec3 contactPoint1 = rb1->pos + normal * rb1->radius;
    glm::vec3 contactPoint2 = rb2->pos - normal * rb2->radius;
    glm::vec3 localContactPoint1 = getLocalPos(rb1, contactPoint1);
    glm::vec3 localContactPoint2 = getLocalPos(rb2, contactPoint2);

    // Relative surface velocity (rb1 relative to rb2)
    glm::vec3 relativeVel = getVelAtPoint(rb1, localContactPoint1) - getVelAtPoint(rb2, localContactPoint2);
    float normalVelMag = glm::dot(relativeVel, normal);
    glm::vec3 normalVel = normal * normalVelMag;
    glm::vec3 tangentialVel = relativeVel - normalVel;

    // Effective combined mass for two-body collision
    float totalMass = rb1->mass + rb2->mass;
    if (totalMass <= 0.00001f) return; // Both static
    float effectiveMass = (rb1->mass * rb2->mass) / totalMass;

    // Penetration depth cap (Prevents spring force spikes)
    float rawPenetration = minDist - dist;
    // Cap maximum allowed spring displacement to 10% of combined radii
    float maxAllowedPenetration = minDist * 0.10f;
    float cappedPenetration = std::min(rawPenetration, maxAllowedPenetration);

    glm::vec3 normalDisplacement = -normal * cappedPenetration;

    float normalStiffness = 10000.0f;
    float normalDampening = 100.0f;

    // Normal spring force
    glm::vec3 normalSpringForce = normalDisplacement * normalStiffness;

    // Damping force clamp (Prevents energy/velocity explosions)
    // Only damp when objects are approaching each other (normalVelMag > 0)
    glm::vec3 normalDampingForce = glm::vec3(0.0f);
    if (normalVelMag > 0.0f) {
        // Maximum force required to completely cancel relative normal velocity in one time step
        float maxStoppingForce = (effectiveMass * normalVelMag) / dt;

        // Raw calculated damping force magnitude
        float rawDampingForceMag = effectiveMass * normalDampening * normalVelMag;

        // Clamp damping force magnitude to prevent over-correction/reversal
        float clampedDampingForceMag = std::min(rawDampingForceMag, maxStoppingForce);

        normalDampingForce = -normal * clampedDampingForceMag;
    }

    // Total normal force
    glm::vec3 normalForce = normalSpringForce + normalDampingForce;

    // Tangential friction & shear clamp
    glm::vec3 tangentialDisplacement = dt * tangentialVel;
    float tangentialStiffness = 200.0f;
    float tangentialDampening = 10.0f;

    glm::vec3 tangentialForce = (tangentialDisplacement * tangentialStiffness) - (effectiveMass * tangentialDampening * tangentialVel);

    // Coulomb friction cone: Base max friction strictly on the normal spring displacement 
    // to avoid force spikes feeding directly into sideways acceleration
    float springNormalForceMag = glm::length(normalSpringForce);
    float maxFrictionForce = friction * springNormalForceMag;

    float tangentialForceMag = glm::length(tangentialForce);
    if (tangentialForceMag > maxFrictionForce && tangentialForceMag > 0.00001f) {
        tangentialForce = (tangentialForce / tangentialForceMag) * maxFrictionForce;
    }

    // Apply forces
    applyForce(rb1, normalForce);
    applyForce(rb2, -normalForce);

    applyForceAtPoint(rb1, tangentialForce, localContactPoint1);
    applyForceAtPoint(rb2, -tangentialForce, localContactPoint2);
}

void CollisionSimulation3D::clear() {
    m_Scene3D->clearEntities();
}

void CollisionSimulation3D::resize(size_t newSphereCount) {
    clear();

    for (size_t i = 0; i < newSphereCount; ++i) {
        float posX = ((rand() % 1000) / 1000.0f) * (2.0f - 0.0f) - 1.0f;
        float posY = ((rand() % 1000) / 1000.0f) * (2.0f - 0.0f) + 1.0f;
        float posZ = ((rand() % 1000) / 1000.0f) * (2.0f - 0.0f) - 1.0f;
        float velX = ((rand() % 10) / 10.0f - 0.5f) * 5.0f;
        float velY = ((rand() % 10) / 10.0f - 0.5f) * 5.0f;
        float velZ = ((rand() % 10) / 10.0f - 0.5f) * 5.0f;

        spawnSphere(glm::vec3(posX, posY, posZ), glm::vec3(velX, velY, velZ), 0.1f);
    }
}

void CollisionSimulation3D::spawnSphere(const glm::vec3& position, const glm::vec3& velocity, float radius) {
    auto newSphere = m_Scene3D->createSphereEntity("Sphere", position, velocity, radius);
    auto* rb = newSphere->getComponent<RigidBody3D>();
    rb->calculateInertia();
}

glm::vec3 getLocalPos(RigidBody3D* rb, glm::vec3 worldPos) {
    return glm::conjugate(rb->orientation) * (worldPos - rb->pos);
}

glm::vec3 getWorldPos(RigidBody3D* rb, glm::vec3 localPos) {
    return rb->pos + rb->orientation * localPos;
}

glm::vec3 getVelAtPoint(RigidBody3D* rb, glm::vec3 localPoint) {
    return rb->vel + rb->orientation * glm::cross(rb->angVel, localPoint);
}