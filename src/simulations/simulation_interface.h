#pragma once

#include <glm/glm.hpp>

enum class Dimension {
	TwoD,
	ThreeD
};

class SimulationInterface {
public:
	virtual ~SimulationInterface() = default;

	// Lifecycle
	virtual void init(float virtualWidth, float virtualHeight, float cellSize) = 0;
	virtual void update(float dt) = 0;
	virtual void render() = 0;
	virtual void clear() = 0;
	virtual void resize(float virtualWidth, float virtualHeight, float cellSize) = 0;

	// Inputs and UI
	virtual void processInput(const glm::vec2& clickPos, bool& isLeftDragging, bool& isRightDragging) = 0;
	virtual void renderUI(float virtualWidth, float virtualHeight, float cellSize) = 0;

	// Tells the main app whether to use Depth Testing, 3D Camera, etc.
	virtual bool is3D() const = 0;

	virtual Dimension getDimension() const = 0;

};