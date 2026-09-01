#pragma once

#include <memory>

#include "core/time.h"
#include "core/input.h"

#include "ui/imgui_layer.h"
#include "ui/panels/profiler_panel.h"

#include "rendering/utils/renderer2d.h"
#include "rendering/utils/renderer3d.h"
#include "physics/2D/spatial_grid.h"

#include "rendering/utils/perspective_camera_controller.h"

#include "physics/physics_engine.h"

const float m_CellSize = 0.375f;

struct GLFWwindow;

class Application {
public:
	Application(const char* title, int width, int height);
	~Application();

	void run();
	void close();

	// Controls
	bool isRunning() const;
	void clearScreen(float r, float g, float b, float a) const;
	void swapBuffers() const;
	void onWindowResize(int width, int height);

	// Window getter
	GLFWwindow* getWindow() const { return m_Window; }

	// Dimension getters
	int getWidth() const { return m_Width; }
	int getHeight() const { return m_Height; }

	void renderCurrentSimulation();

private:
	void update(float dt);
	void render();
	void processInput();

	void initWindow(const char* title);

	GLFWwindow* m_Window;
	int m_Width;
	int m_Height;

	// Virtual world size
	float m_VirtualHeight = 4.5f;
	float m_VirtualWidth;

	// Important objects
	std::unique_ptr<Time> m_Timer;

	std::unique_ptr<ProfilerPanel> m_ProfilerPanel;
	std::unique_ptr<ImGuiLayer> m_ImGuiLayer;

	std::unique_ptr<Renderer2D> m_Renderer2D;
	std::unique_ptr<Renderer3D> m_Renderer3D;
	std::unique_ptr<SpatialGrid> m_SpatialGrid;

	std::unique_ptr<PerspectiveCameraController> m_CameraController;

	std::unique_ptr<PhysicsEngine> m_PhysicsEngine;

	bool isLeftDragging = false;
	bool isRightDragging = false;
};