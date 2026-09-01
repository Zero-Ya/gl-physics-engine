#pragma once

#include "physics/2D/spatial_grid.h"

#include "ui/panels/profiler_panel.h"
#include "rendering/utils/renderer2d.h"
#include "rendering/utils/renderer3d.h"

#include "simulations/simulation_interface.h"
#include "simulations/adapters/collision_simulation2d_adapter.h"
#include "simulations/adapters/cloth_simulation2d_adapter.h"

enum class SimulationType {
	None,
	Collision2D,
	Cloth2D,
};

class PhysicsEngine {
public:
    PhysicsEngine(SpatialGrid& spatialGrid, ProfilerPanel& profilerPanel, Renderer2D& renderer2D, Renderer3D& renderer3D);
    ~PhysicsEngine() = default;

    void init(float virtualWidth, float virtualHeight, float cellSize) {
        if (m_CurrentSim) {
            m_CurrentSim->init(virtualWidth, virtualHeight, cellSize);
        }
    }

    void update(float dt) {
        if (m_CurrentSim) {
            m_CurrentSim->update(dt);
        }
    }

    void render() {
        if (m_CurrentSim) {
            m_CurrentSim->render();
        }
    }

    void processInput(const glm::vec2& clickPos, bool& isLeftDragging, bool& isRightDragging) {
        if (m_CurrentSim) {
            m_CurrentSim->processInput(clickPos, isLeftDragging, isRightDragging);
        }
    }

    void resize(float virtualWidth, float virtualHeight, float cellSize) {
        if (m_CurrentSim) {
            m_CurrentSim->resize(virtualWidth, virtualHeight, cellSize);
        }
    }

    void switchSimulation(SimulationType newType, float virtualWidth, float virtualHeight, float cellSize);
    void renderUI(float virtualWidth, float virtualHeight, float cellSize);

    // Getters
    SimulationType getSimulationType() const { return m_SimulationType; }
    SimulationInterface* getCurrentSimulation() { return m_CurrentSim.get(); }

private:
    SpatialGrid& m_SpatialGrid;
    ProfilerPanel& m_ProfilerPanel;
    Renderer2D& m_Renderer2D;
    Renderer3D& m_Renderer3D;

    SimulationType m_SimulationType = SimulationType::None;
    std::unique_ptr<SimulationInterface> m_CurrentSim; // Polymorphic active simulation
};