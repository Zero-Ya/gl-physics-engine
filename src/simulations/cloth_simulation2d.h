#pragma once

#include <vector>
#include <chrono>
#include <glm/glm.hpp>

// Individual point-mass in the cloth grid
struct ClothParticle {
    glm::vec2 position { 0.0f, 0.0f };
    glm::vec2 oldPosition { 0.0f, 0.0f };
    glm::vec2 acceleration { 0.0f, 0.0f };
    bool isPinned{ false };
};

// Distance constraint connecting two particles
struct ClothConstraint {
    size_t particleA;
    size_t particleB;
    float restDistance;
    bool isActive{ true }; // For tearing
};

class ClothSimulation2D {
public:
    ClothSimulation2D() = default;
    ~ClothSimulation2D() = default;

    // Core control
    void init(size_t width, size_t height, float spacing, const glm::vec2& startPos);
    void update(float dt);
    void clear();
    void resize(float virtualWidth, float virtualHeight, float coverageWidthPercent, float newSpacing);

    // Pin / unpin
    void pinParticle(size_t x, size_t y);
    void unpinParticle(size_t x, size_t y);

    // Mouse dragging and tearing
    int findNearestParticle(const glm::vec2& mousePos, float maxRadius);
    void startDragging(const glm::vec2& mousePos);
    void updateDragging(const glm::vec2& mousePos);
    void stopDragging();
    void tearAtPosition(const glm::vec2& tearPos, float radius);

    // Getters for UI panels or rendering
    const std::vector<ClothParticle>& getParticles() const { return m_Particles; }
    const std::vector<ClothConstraint>& getConstraints() const { return m_Constraints; }
    float getPhysicsTimeMs() const { return m_PhysicsTimeMs; }
    size_t getParticleCount() const { return m_Particles.size(); }

    // Parameters
    int m_SolverIterations = 3; // Higher = stiffer cloth, lower = stretchy
    glm::vec2 m_Gravity { 0.0f, -9.81f };
    void setSolverIterations(const int& iterations) { m_SolverIterations = iterations; }
    void setGravity(const glm::vec2& gravity) { m_Gravity = gravity; }

    // Other cloth settings
    void setAutoTear(bool boolVal) { m_EnableAutoTear = boolVal; }

private:
    void applyForces();
    void verletIntegrate(float dt);
    void satisfyConstraints();
    void resolveWorldBoundaries();

private:
    std::vector<ClothParticle>   m_Particles;
    std::vector<ClothConstraint> m_Constraints;

    uint32_t m_DraggedParticleIndex;

    size_t m_GridWidth = 0;
    size_t m_GridHeight = 0;
    float  m_Spacing = 0.5f;

    float m_PhysicsTimeMs = 0.0f;

    bool m_EnableAutoTear = false;
};