#include "physics_engine.h"

#include <glfw/glfw3.h>
#include <imgui.h>

#include "core/input.h"

PhysicsEngine::PhysicsEngine(SpatialGrid& spatialGrid, ProfilerPanel& profilerPanel, Renderer2D& renderer2D, Renderer3D& renderer3D)
    : m_SpatialGrid(spatialGrid), m_ProfilerPanel(profilerPanel), m_Renderer2D(renderer2D), m_Renderer3D(renderer3D)
{}

void PhysicsEngine::switchSimulation(SimulationType newType, float virtualWidth, float virtualHeight, float cellSize) {
    if (m_SimulationType == newType) return;

    // Cleanup old simulation
    if (m_CurrentSim) {
        m_CurrentSim->clear();
        m_CurrentSim.reset();
    }

    m_SimulationType = newType;

    // Instantiate new simulation type
    switch (m_SimulationType) {
    case SimulationType::Collision2D:
         m_CurrentSim = std::make_unique<CollisionSimulation2DAdapter>(m_SpatialGrid, m_Renderer2D);
        break;

    case SimulationType::Cloth2D:
        m_CurrentSim = std::make_unique<ClothSimulation2DAdapter>(m_Renderer2D);
        break;

    case SimulationType::None:
    default:
        m_CurrentSim = nullptr;
        return;
    }

    // Initialize fresh state
    if (m_CurrentSim) {
        m_CurrentSim->init(virtualWidth, virtualHeight, cellSize);
    }
}

void PhysicsEngine::renderUI(float virtualWidth, float virtualHeight, float cellSize) {
    ImGui::Begin("Simulation Control");

    // Dropdown combo box selection
    const char* simulationNames[] = { "None", "2D Collision Simulation", "2D Cloth Simulation" };
    int currentSimulation = static_cast<int>(m_SimulationType);

    ImGui::Text("Active Simulation:");
    if (ImGui::Combo("##Combo", &currentSimulation, simulationNames, IM_ARRAYSIZE(simulationNames))) {
        switchSimulation(static_cast<SimulationType>(currentSimulation), virtualWidth, virtualHeight, cellSize);
    }

    ImGui::Separator();

    if (m_CurrentSim) {
        m_CurrentSim->renderUI(virtualWidth, virtualHeight, cellSize);
    }

    ImGui::End();
}