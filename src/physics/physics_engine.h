#pragma once

#include "ui/panels/profiler_panel.h"

#include "physics/2D/spatial_grid.h"
#include "physics/2D/spatial_grid_debug_renderer.h"
#include "rendering/utils/renderer2d.h"

#include "simulations/collision_simulation2d.h"
#include "simulations/cloth_simulation2d.h"

enum class SimulationType {
	None,
	Collision2D,
	Cloth2D,
};

class PhysicsEngine {
public:
	PhysicsEngine(SpatialGrid& spatialGrid, Renderer2D& renderer, ProfilerPanel& profilerPanel);
	~PhysicsEngine() = default;

	void init();
	void update(float dt);
	void render();
	void clear();
	void resize(float virtualWidth, float virtualHeight, float cellSize);
	void switchSimulation(SimulationType newType, float virtualWidth, float virtualHeight, float cellSize);
	void processInput(glm::vec2 clickPos, bool& isLeftDragging, bool& isRightDragging);

	void renderProfilerPanel(float lastTotalFrameTimeMs);
	void renderSimulationControl(float virtualWidth, float virtualHeight, float cellSize);

	// Simulation getters
	const SimulationType& getSimulationType() const { return m_SimulationType; }
	std::unique_ptr<CollisionSimulation2D>& getCollisionEngine() { return m_CollisionEngine; }
	std::unique_ptr<ClothSimulation2D>& getClothEngine() { return m_ClothEngine; }

private:
	SpatialGrid& m_SpatialGrid;
	Renderer2D& m_Renderer;
	ProfilerPanel& m_ProfilerPanel;

	SimulationType m_SimulationType = SimulationType::None;

	std::unique_ptr<CollisionSimulation2D> m_CollisionEngine;
	std::unique_ptr<ClothSimulation2D> m_ClothEngine;
};