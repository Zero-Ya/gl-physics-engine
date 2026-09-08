#pragma once

#include <imgui.h>
#include <glfw/glfw3.h>

#include "simulations/simulation_interface.h"
#include "simulations/3D/collision_simulation3d.h"

#include "rendering/utils/renderer3d.h"
#include "core/input.h"

class CollisionSimulation3DAdapter : public SimulationInterface {
public:
    CollisionSimulation3DAdapter(Renderer3D& renderer) : m_Renderer(renderer), m_CollisionSolver() {}

    void init(float virtualWidth, float virtualHeight, float cellSize) override {
        m_CollisionSolver.init(15);
    }

    void update(float dt) override {
        m_CollisionSolver.update(dt);
    }

    void render() override {
        m_Renderer.drawSpheres(m_CollisionSolver.getSpheres());
    }

    void clear() override {
        m_CollisionSolver.clear();
    }

    void resize(float virtualWidth, float virtualHeight, float cellSize) override {

    }

    void processInput(const glm::vec2& clickPos, bool& isLeftDragging, bool& isRightDragging) override {
        // Torque in X axis
        if (Input::isKeyHeld(GLFW_KEY_1)) {
            m_CollisionSolver.applyTorques(glm::vec3(1.0f, 0.0f, 0.0f));
        }

        // Torque in Y axis
        if (Input::isKeyHeld(GLFW_KEY_2)) {
            m_CollisionSolver.applyTorques(glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // Torque in Z axis
        if (Input::isKeyHeld(GLFW_KEY_3)) {
            m_CollisionSolver.applyTorques(glm::vec3(0.0f, 0.0f, 1.0f));
        }

        if (Input::isKeyHeld(GLFW_KEY_UP)) {
            m_CollisionSolver.applyForces(glm::vec3(0.0f, 0.0f, -100.0f));
        }
        if (Input::isKeyHeld(GLFW_KEY_DOWN)) {
            m_CollisionSolver.applyForces(glm::vec3(0.0f, 0.0f, 100.0f));
        }
        if (Input::isKeyHeld(GLFW_KEY_RIGHT)) {
            m_CollisionSolver.applyForces(glm::vec3(100.0f, 0.0f, 0.0f));
        }
        if (Input::isKeyHeld(GLFW_KEY_LEFT)) {
            m_CollisionSolver.applyForces(glm::vec3(-100.0f, 0.0f, 0.0f));
        }

        // Force in Y axis
        if (Input::isKeyHeld(GLFW_KEY_SPACE)) {
            m_CollisionSolver.applyForces(glm::vec3(0.0f, 200.0f, 0.0f));
        }
    }

    void renderUI(float virtualWidth, float virtualHeight, float cellSize) override {
        ImGui::Begin("Simulation Control");

        static float gravityConst = -9.81f;
        ImGui::Text("Gravity:");
        ImGui::SliderFloat("##Gravity", &gravityConst, -10.0f, 10.0f, "%.2f");
        m_CollisionSolver.setGravity(glm::vec3(0.0f, gravityConst, 0.0f));

        static int sphereCounts = 15;
        ImGui::Text("Amount of spheres:");
        if (ImGui::SliderInt("##Amount of spheres", &sphereCounts, 1, 100))
            m_CollisionSolver.resize(sphereCounts);

        ImGui::End();
    }

    void renderProfilerPanel(ProfilerPanel& profilerPanel, float lastTotalFrameTimeMs) override {
        profilerPanel.onImGuiRender(lastTotalFrameTimeMs, m_CollisionSolver.getPhysicsTimeMs(), m_CollisionSolver.getEntityCount(), false);
    }

    bool is3D() const override { return true; }

    Dimension getDimension() const override { return Dimension::ThreeD; }

private:
    CollisionSimulation3D m_CollisionSolver; // Internal solver
    Renderer3D& m_Renderer;

};