#include "collision_solver2d.h"
#include "scene/game_object.h"
#include "components/2D/transform2d.h"
#include "components/2D/rigidbody2d.h"

void CollisionSolver2D::resolveCircleCollision(GameObject* objA, GameObject* objB) {
    auto* tfA = objA->getComponent<Transform2D>();
    auto* rbA = objA->getComponent<RigidBody2D>();
    auto* tfB = objB->getComponent<Transform2D>();
    auto* rbB = objB->getComponent<RigidBody2D>();

    if (!tfA || !rbA || !tfB || !rbB) return;

    // Hardcoded stuff
    float radiusA = 0.5f;
    float radiusB = 0.5f;
    float restitution = 0.8f; // Bounciness

    // 1. Detection phase
    glm::vec2 distanceVec = tfB->position - tfA->position;
    float distance = glm::length(distanceVec);
    float totalRadius = radiusA + radiusB;

    if (distance >= totalRadius || distance == 0.0f) return;

    float penetration = totalRadius - distance;
    // Contact normal vector
    glm::vec2 normal = distanceVec / distance;

    // 2. Position correction phase
    float totalInverseMass = rbA->inverseMass + rbB->inverseMass;
    if (totalInverseMass == 0.0f) return;

    glm::vec2 correction = normal * (penetration / totalInverseMass);
    tfA->position -= correction * rbA->inverseMass;
    tfB->position += correction * rbB->inverseMass;

    // 3. Impulse resolution phase
    glm::vec2 relativeVelocity = rbB->velocity - rbA->velocity;
    float velAlongNormal = glm::dot(relativeVelocity, normal);

    if (velAlongNormal > 0) return;

    float impulseMagnitude = -(1.0f + restitution) * velAlongNormal;
    impulseMagnitude /= totalInverseMass;

    glm::vec2 impulseVector = impulseMagnitude * normal;
    rbA->velocity -= rbA->inverseMass * impulseVector;
    rbB->velocity += rbB->inverseMass * impulseVector;
}
