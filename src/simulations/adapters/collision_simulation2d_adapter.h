#pragma once

#include <imgui.h>
#include <glfw/glfw3.h>

#include "simulations/simulation_interface.h"
#include "simulations/2D/collision_simulation2d.h"

#include "rendering/utils/renderer2d.h"
#include "core/input.h"

class CollisionSimulation2DAdapter : public SimulationInterface {
public:
    CollisionSimulation2DAdapter(SpatialGrid& spatialGrid, Renderer2D& renderer) : m_Renderer(renderer), m_CollisionSolver(spatialGrid) {}

    void init(float virtualWidth, float virtualHeight, float cellSize) override {
        m_CollisionSolver.init(100);
        m_CollisionSolver.updateSpatialGridAndWorldBounds(virtualWidth, virtualHeight, cellSize);
    }

    void update(float dt) override {
        m_CollisionSolver.update(dt);
    }

    void render() override {
        m_Renderer.drawCircles(m_CollisionSolver.getCircles());
    }

    void clear() override {
        m_CollisionSolver.clear();
    }

    void resize(float virtualWidth, float virtualHeight, float cellSize) override {
        m_CollisionSolver.updateSpatialGridAndWorldBounds(virtualWidth, virtualHeight, cellSize);
    }

    void processInput(const glm::vec2& clickPos, bool& isLeftDragging, bool& isRightDragging) override {
        if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            m_CollisionSolver.spawnCircle(clickPos, glm::vec2(0.0f, -10.0f), 0.1f);
        }
    }

    void renderUI(float virtualWidth, float virtualHeight, float cellSize) override {
        ImGui::Begin("Simulation Control");

        static float gravityConst = 0.0f;
        static float forceConst = 0.0f;

        ImGui::Text("Gravity:");
        ImGui::SliderFloat("##Gravity", &gravityConst, -10.0f, 10.0f, "%.2f");

        ImGui::Text("Horizontal force:");
        ImGui::SliderFloat("##Horizontal force", &forceConst, -10.0f, 10.0f, "%.2f");
        m_CollisionSolver.setGravity(glm::vec2(forceConst, gravityConst));

        ImGui::End();
    }

    bool is3D() const override { return true; }

    Dimension getDimension() const override { return Dimension::TwoD; }

private:
    CollisionSimulation2D m_CollisionSolver; // Internal solver
    Renderer2D& m_Renderer;

};