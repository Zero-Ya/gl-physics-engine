#include "physics_engine.h"

#include <glfw/glfw3.h>
#include <imgui.h>

#include "core/input.h"

PhysicsEngine::PhysicsEngine(SpatialGrid& spatialGrid, Renderer2D& renderer, ProfilerPanel& profilerPanel)
    : m_SpatialGrid(spatialGrid), m_Renderer(renderer), m_ProfilerPanel(profilerPanel)
{}

void PhysicsEngine::init() {
    m_CollisionEngine = std::make_unique<CollisionSimulation2D>(m_SpatialGrid);
    m_ClothEngine = std::make_unique<ClothSimulation2D>();

    // Initialize state for selected simulation
    switch (m_SimulationType) {
    case SimulationType::Collision2D:
        m_CollisionEngine->init(100);
        m_CollisionEngine->updateSpatialGridAndWorldBounds(6.0f, 4.5f, 0.375f);
        break;

    case SimulationType::Cloth2D:
        m_ClothEngine->init(10, 12, 0.25, glm::vec2(2.0f, 4.0f));
        break;

    default:
        break;
    }
}

void PhysicsEngine::update(float dt) {
    // Update physics
    switch (m_SimulationType) {
    case SimulationType::Collision2D:
        m_CollisionEngine->update(dt);
        break;

    case SimulationType::Cloth2D:
        m_ClothEngine->update(dt);
        break;

    default:
        break;
    }
}

void PhysicsEngine::render() {
    // Render
    switch (m_SimulationType) {
    case SimulationType::Collision2D:
        m_Renderer.drawCircles(m_CollisionEngine->getCircles());
        break;

    case SimulationType::Cloth2D:
        for (const auto& constraint : m_ClothEngine->getConstraints()) {
            if (constraint.isActive == true) {
                const auto& p1 = m_ClothEngine->getParticles()[constraint.particleA].position;
                const auto& p2 = m_ClothEngine->getParticles()[constraint.particleB].position;

                m_Renderer.drawLine(p1, p2, glm::vec3(0.8f, 0.8f, 0.8f));
            }
        }

        // Draw pinned particle points
        for (const auto& particle : m_ClothEngine->getParticles()) {
            if (particle.isPinned == true)
                m_Renderer.drawCircle(particle.position, glm::vec3(1.0f, 1.0f, 1.0f), 0.035f);
        }
        break;

    default:
        break;
    }
}

void PhysicsEngine::clear() {
    // Cleanup current simulation
    switch (m_SimulationType) {
    case SimulationType::Collision2D:
        m_CollisionEngine->clear();
        break;

    case SimulationType::Cloth2D:
        m_ClothEngine->clear();
        break;

    default:
        break;
    }
}

void PhysicsEngine::resize(float virtualWidth, float virtualHeight, float cellSize) {
    // Resize current simulation
    switch (m_SimulationType) {
    case SimulationType::Collision2D:
        m_CollisionEngine->updateSpatialGridAndWorldBounds(virtualWidth, virtualHeight, cellSize);
        break;

    case SimulationType::Cloth2D:
        m_ClothEngine->resize(virtualWidth, virtualHeight, 0.40f, 0.15f);
        break;

    default:
        break;
    }
}

// This is temporary...
void PhysicsEngine::switchSimulation(SimulationType newType, float virtualWidth, float virtualHeight, float cellSize) {
    if (m_SimulationType == newType) return;
    clear();

    m_SimulationType = newType;
    init();
    resize(virtualWidth, virtualHeight, cellSize);
}

void PhysicsEngine::processInput(glm::vec2 clickPos, bool& isLeftDragging, bool& isRightDragging) {
    switch (m_SimulationType) {
    case SimulationType::Collision2D:
        if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            m_CollisionEngine->spawnCircle(clickPos, glm::vec2(0.0f, -10.0f), 0.1f);
        }

        break;

    case SimulationType::Cloth2D:
        // Left click: Grab and drag
        if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
            if (!isLeftDragging) {
                m_ClothEngine->startDragging(clickPos);
                isLeftDragging = true;
            }
            else {
                m_ClothEngine->updateDragging(clickPos);
            }
        }
        else {
            m_ClothEngine->stopDragging();
            isLeftDragging = false;
        }

        // Right click: Drag to tear cloth
        if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (!isRightDragging) {
                isRightDragging = true;
            }
            else {
                m_ClothEngine->tearAtPosition(clickPos, 0.2f); // 0.2 radius cut zone
            }
        }
        else {
            isRightDragging = false;
        }
        break;

    default:
        break;
    }

}

void PhysicsEngine::renderProfilerPanel(float lastTotalFrameTimeMs) {
    switch (m_SimulationType) {
    case SimulationType::Collision2D:
        if (m_ProfilerPanel.isDebugGridEnabled()) {
            SpatialGridDebugRenderer::draw(m_SpatialGrid, *m_Renderer.getLineRenderer().get(), true);
        }
        m_ProfilerPanel.onImGuiRender(lastTotalFrameTimeMs, m_CollisionEngine->getPhysicsTimeMs(), m_CollisionEngine->getEntityCount(), true);
        break;

    case SimulationType::Cloth2D:
        m_ProfilerPanel.onImGuiRender(lastTotalFrameTimeMs, m_ClothEngine->getPhysicsTimeMs(), m_ClothEngine->getParticleCount(), false);
        break;

    default:
        break;
    }
}

void PhysicsEngine::renderSimulationControl(float virtualWidth, float virtualHeight, float cellSize) {
    ImGui::Begin("Simulation Control");

    // Dropdown combo box selection
    const char* simulationNames[] = { "None", "2D Collision Simulation", "2D Cloth Simulation" };
    int currentSimulation = static_cast<int>(m_SimulationType);

    ImGui::Text("Active Simulation:");
    if (ImGui::Combo("##Combo", &currentSimulation, simulationNames, IM_ARRAYSIZE(simulationNames))) {
        switchSimulation(static_cast<SimulationType>(currentSimulation), virtualWidth, virtualHeight, cellSize);
    }

    ImGui::Separator();
    ImGui::Spacing();

    // Display active simulation controls
    // Collision 2D
    if (m_SimulationType == SimulationType::Collision2D) {
        static float gravityConst = 0.0f;
        static float forceConst = 0.0f;

        ImGui::Text("Gravity:");
        ImGui::SliderFloat("##Gravity", &gravityConst, -10.0f, 10.0f, "%.2f");

        ImGui::Text("Horizontal force:");
        ImGui::SliderFloat("##Horizontal force", &forceConst, -10.0f, 10.0f, "%.2f");
        m_CollisionEngine->setGravity(glm::vec2(forceConst, gravityConst));
    }
    // // //

    // Cloth 2D
    if (m_SimulationType == SimulationType::Cloth2D) {
        float coverageWidthPercent = 0.40f;
        float newSpacing = 0.15f;

        ImGui::Spacing();

        static int solverIterations = 3; // Higher = stiffer cloth, lower = stretchy
        static float gravityConst = -9.81f;

        ImGui::Text("Solver Iterations:");
        ImGui::SliderInt("##Solver Iterations", &solverIterations, 1, 5);
        m_ClothEngine->setSolverIterations(solverIterations);

        ImGui::Text("Gravity:");
        ImGui::SliderFloat("##Gravity", &gravityConst, 0.0f, -10.0f, "%.2f");
        m_ClothEngine->setGravity(glm::vec2(0.0f, gravityConst));

        ImGui::Spacing();

        static bool autoTearBool = false;
        ImGui::Checkbox("Enable Auto Tear", &autoTearBool);
        m_ClothEngine->setAutoTear(autoTearBool);

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Pin top-corner particles")) {
            float desiredClothWidth = virtualWidth * coverageWidthPercent;
            size_t newGridWidth = static_cast<size_t>(std::max(2.0f, std::round(desiredClothWidth / newSpacing)));

            m_ClothEngine->pinParticle(0, 0);
            m_ClothEngine->pinParticle(newGridWidth - 1, 0);
        }

        ImGui::Spacing();

        if (ImGui::Button("Unpin all particles")) {
            float desiredClothWidth = virtualWidth * coverageWidthPercent;
            float desiredClothHeight = desiredClothWidth * 1.0f;

            size_t newGridWidth = static_cast<size_t>(std::max(2.0f, std::round(desiredClothWidth / newSpacing)));
            size_t newGridHeight = static_cast<size_t>(std::max(2.0f, std::round(desiredClothHeight / newSpacing)));

            for (size_t y = 0; y < newGridHeight; ++y) {
                for (size_t x = 0; x < newGridWidth; ++x) {
                    m_ClothEngine->unpinParticle(x, y);
                }
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Reset cloth")) {
            m_ClothEngine->resize(virtualWidth, virtualHeight, coverageWidthPercent, newSpacing);
        }
    }
    // // // 

    ImGui::End();
}
