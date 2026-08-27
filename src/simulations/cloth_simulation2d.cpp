#include "cloth_simulation2d.h"
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

void ClothSimulation2D::init(size_t width, size_t height, float spacing, const glm::vec2& startPos) {
    clear();
    m_GridWidth = width;
    m_GridHeight = height;
    m_Spacing = spacing;

    m_Particles.reserve(width * height);

    m_DraggedParticleIndex = -1;

    // 1. Generate Particle Grid
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            ClothParticle p;
            p.position = startPos + glm::vec2(x * spacing, -static_cast<float>(y) * spacing);
            p.oldPosition = p.position;
            p.acceleration = glm::vec2(0.0f);
            p.isPinned = false;

            m_Particles.push_back(p);
        }
    }

    // Pin top-left and top-right corners by default
    pinParticle(0, 0);
    pinParticle(width - 1, 0);

    // Helper lambda to convert (x,y) grid coordinate to 1D index
    auto getIndex = [width](size_t x, size_t y) { return y * width + x; };

    // 2. Build Constraints (Structural, Shear, Bend)
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t current = getIndex(x, y);

            // Structural constraints (1-unit neighbors)
            if (x < width - 1) { // Right neighbors
                size_t right = getIndex(x + 1, y);
                m_Constraints.push_back({ current, right, spacing, true });
            }
            if (y < height - 1) { // Bottom neighbors
                size_t bottom = getIndex(x, y + 1);
                m_Constraints.push_back({ current, bottom, spacing, true });
            }

            // Shear constraints (Diagonals neigbors)
            //if (x < width - 1 && y < height - 1) {
            //    size_t bottomRight = getIndex(x + 1, y + 1);
            //    float diagDist = std::sqrt(spacing * spacing + spacing * spacing);
            //    m_Constraints.push_back({ current, bottomRight, diagDist, true });
            //}
            //if (x > 0 && y < height - 1) {
            //    size_t bottomLeft = getIndex(x - 1, y + 1);
            //    float diagDist = std::sqrt(spacing * spacing + spacing * spacing);
            //    m_Constraints.push_back({ current, bottomLeft, diagDist, true });
            //}

            // Bend constraints (2-units neigbors)
            if (x < width - 2) {
                size_t right2 = getIndex(x + 2, y);
                m_Constraints.push_back({ current, right2, spacing * 2.0f, true });
            }
            if (y < height - 2) {
                size_t bottom2 = getIndex(x, y + 2);
                m_Constraints.push_back({ current, bottom2, spacing * 2.0f, true });
            }
        }
    }
}

void ClothSimulation2D::clear() {
    m_Particles.clear();
    m_Constraints.clear();
}

void ClothSimulation2D::resize(float virtualWidth, float virtualHeight, float coverageWidthPercent, float newSpacing) {
    // Determine desired world dimensions of the cloth
    float desiredClothWidth = virtualWidth * coverageWidthPercent;
    float desiredClothHeight = desiredClothWidth * 1.0f; // aspect ratio for fabric

    // Compute grid resolution (discrete width/height)
    size_t newGridWidth = static_cast<size_t>(std::max(2.0f, std::round(desiredClothWidth / newSpacing)));
    size_t newGridHeight = static_cast<size_t>(std::max(2.0f, std::round(desiredClothHeight / newSpacing)));

    // Center the cloth starting position horizontally near the top edge
    float startX = (virtualWidth - (newGridWidth - 1) * newSpacing) * 0.5f;
    float startY = virtualHeight - 1.0f; // 1 unit below top edge

    init(newGridWidth, newGridHeight, newSpacing, glm::vec2(startX, startY));
}

void ClothSimulation2D::pinParticle(size_t x, size_t y) {
    if (x < m_GridWidth && y < m_GridHeight) {
        m_Particles[y * m_GridWidth + x].isPinned = true;
    }
}

void ClothSimulation2D::unpinParticle(size_t x, size_t y) {
    if (x < m_GridWidth && y < m_GridHeight) {
        m_Particles[y * m_GridWidth + x].isPinned = false;
    }
}

void ClothSimulation2D::update(float dt) {
    auto startTime = std::chrono::high_resolution_clock::now();

    //dt = std::min(dt, 0.0005f);

    // Apply environmental forces (Gravity, wind ...later)
    applyForces();

    // Perform Verlet integration to step positions forward
    verletIntegrate(dt);

    // Relax distance constraints over multiple relaxation passes
    for (int i = 0; i < m_SolverIterations; ++i) {
        satisfyConstraints();
        resolveWorldBoundaries();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    m_PhysicsTimeMs = std::chrono::duration<float, std::milli>(endTime - startTime).count();
}

void ClothSimulation2D::applyForces() {
    for (auto& p : m_Particles) {
        if (!p.isPinned) {
            p.acceleration += m_Gravity;
        }
    }
}

void ClothSimulation2D::verletIntegrate(float dt) {
    float damping = 0.99f; // Air resistance simulation
    for (auto& p : m_Particles) {
        if (p.isPinned) continue;

        glm::vec2 velocity = (p.position - p.oldPosition) * damping;
        p.oldPosition = p.position;
        p.position += velocity + p.acceleration * (dt * dt);
        p.acceleration = glm::vec2(0.0f); // Reset acceleration for next frame
    }
}

void ClothSimulation2D::satisfyConstraints() {
    const float tearFactor = 2.0f; // Tears if stretched to 2x its rest length

    for (auto& c : m_Constraints) {
        if (!c.isActive) continue;

        ClothParticle& pA = m_Particles[c.particleA];
        ClothParticle& pB = m_Particles[c.particleB];

        glm::vec2 delta = pB.position - pA.position;
        float currentDist = glm::length(delta);

        // Automatic tearing
        if (m_EnableAutoTear) {
            if (currentDist > c.restDistance * tearFactor) {
                c.isActive = false;
                continue;
            }
        }

        if (currentDist > 0.0001f) {
            float error = (currentDist - c.restDistance) / currentDist;
            glm::vec2 correction = delta * 0.5f * error;

            if (!pA.isPinned && !pB.isPinned) {
                pA.position += correction;
                pB.position -= correction;
            }
            else if (!pA.isPinned) {
                pA.position += correction * 2.0f;
            }
            else if (!pB.isPinned) {
                pB.position -= correction * 2.0f;
            }
        }
    }
}

void ClothSimulation2D::resolveWorldBoundaries() {
    // Floor collision at y = floorY
    float floorY = -0.0f;
    for (auto& p : m_Particles) {
        if (!p.isPinned && p.position.y < floorY) {
            p.position.y = floorY;
        }
    }
}

//
//
//

// Finds the nearest particle within a maximum pick radius (e.g., radius = 1.0f)
int ClothSimulation2D::findNearestParticle(const glm::vec2& mousePos, float maxRadius) {
    int closestIndex = -1;
    float minSqDist = maxRadius * maxRadius;

    for (size_t i = 0; i < m_Particles.size(); ++i) {
        float sqDist = glm::distance2(mousePos, m_Particles[i].position);
        if (sqDist < minSqDist) {
            minSqDist = sqDist;
            closestIndex = static_cast<int>(i);
        }
    }
    return closestIndex;
}

// Begins dragging a particle
void ClothSimulation2D::startDragging(const glm::vec2& mousePos) {
    m_DraggedParticleIndex = findNearestParticle(mousePos, 1.0f);
}

// Updates the dragged particle's position to track the mouse cursor
void ClothSimulation2D::updateDragging(const glm::vec2& mousePos) {
    if (m_DraggedParticleIndex != -1) {
        ClothParticle& p = m_Particles[m_DraggedParticleIndex];
        p.position = mousePos;
        p.oldPosition = mousePos; // Prevents velocity spikes when released
    }
}

// Releases the dragged particle
void ClothSimulation2D::stopDragging() {
    m_DraggedParticleIndex = -1;
}

// Tears (deactivates) constraints passing near a cut point (e.g., right-click)
void ClothSimulation2D::tearAtPosition(const glm::vec2& tearPos, float radius) {
    float sqRadius = radius * radius;

    for (auto& c : m_Constraints) {
        if (!c.isActive) continue;

        const glm::vec2& posA = m_Particles[c.particleA].position;
        const glm::vec2& posB = m_Particles[c.particleB].position;

        // Check distance from tear point to constraint midpoint
        glm::vec2 midPoint = (posA + posB) * 0.5f;
        if (glm::distance2(tearPos, midPoint) < sqRadius) {
            c.isActive = false; // Tear the constraint link
        }
    }
}