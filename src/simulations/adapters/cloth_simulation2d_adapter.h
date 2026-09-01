#pragma once

#include <imgui.h>
#include <glfw/glfw3.h>

#include "simulations/simulation_interface.h"
#include "simulations/2D/cloth_simulation2d.h"

#include "rendering/utils/renderer2d.h"
#include "core/input.h"

class ClothSimulation2DAdapter : public SimulationInterface {
public:

    ClothSimulation2DAdapter(Renderer2D& renderer) : m_Renderer(renderer) {}

    void init(float virtualWidth, float virtualHeight, float cellSize) override {
        m_ClothSolver.init(10, 12, 0.25, glm::vec2(2.0f, 4.0f));
        m_ClothSolver.resize(virtualWidth, virtualHeight, 0.40f, 0.15f);
    }

    void update(float dt) override {
        m_ClothSolver.update(dt);
    }

    void render() override {
        for (const auto& constraint : m_ClothSolver.getConstraints()) {
            if (constraint.isActive == true) {
                const auto& p1 = m_ClothSolver.getParticles()[constraint.particleA].position;
                const auto& p2 = m_ClothSolver.getParticles()[constraint.particleB].position;

                m_Renderer.drawLine(p1, p2, glm::vec3(0.8f, 0.8f, 0.8f));
            }
        }

        // Draw pinned particle points
        for (const auto& particle : m_ClothSolver.getParticles()) {
            if (particle.isPinned == true)
                m_Renderer.drawCircle(particle.position, glm::vec3(1.0f, 1.0f, 1.0f), 0.035f);
        }
    }

    void clear() override {
        m_ClothSolver.clear();
    }

    void resize(float virtualWidth, float virtualHeight, float cellSize) override {
        m_ClothSolver.resize(virtualWidth, virtualHeight, 0.40f, 0.15f);
    }

    void processInput(const glm::vec2& clickPos, bool& isLeftDragging, bool& isRightDragging) override {
        // Left click: Grab and drag
        if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
            if (!isLeftDragging) {
                m_ClothSolver.startDragging(clickPos);
                isLeftDragging = true;
            }
            else {
                m_ClothSolver.updateDragging(clickPos);
            }
        }
        else {
            m_ClothSolver.stopDragging();
            isLeftDragging = false;
        }

        // Right click: Drag to tear cloth
        if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) {
            if (!isRightDragging) {
                isRightDragging = true;
            }
            else {
                m_ClothSolver.tearAtPosition(clickPos, 0.2f); // 0.2 radius cut zone
            }
        }
        else {
            isRightDragging = false;
        }
    }

    void renderUI(float virtualWidth, float virtualHeight, float cellSize) override {
        ImGui::Begin("Simulation Control");

        float coverageWidthPercent = 0.40f;
        float newSpacing = 0.15f;

        ImGui::Spacing();

        static int solverIterations = 3; // Higher = stiffer cloth, lower = stretchy
        static float gravityConst = -9.81f;

        ImGui::Text("Solver Iterations:");
        ImGui::SliderInt("##Solver Iterations", &solverIterations, 1, 5);
        m_ClothSolver.setSolverIterations(solverIterations);

        ImGui::Text("Gravity:");
        ImGui::SliderFloat("##Gravity", &gravityConst, 0.0f, -10.0f, "%.2f");
        m_ClothSolver.setGravity(glm::vec2(0.0f, gravityConst));

        ImGui::Spacing();

        static bool autoTearBool = false;
        ImGui::Checkbox("Enable Auto Tear", &autoTearBool);
        m_ClothSolver.setAutoTear(autoTearBool);

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Pin top-corner particles")) {
            float desiredClothWidth = virtualWidth * coverageWidthPercent;
            size_t newGridWidth = static_cast<size_t>(std::max(2.0f, std::round(desiredClothWidth / newSpacing)));

            m_ClothSolver.pinParticle(0, 0);
            m_ClothSolver.pinParticle(newGridWidth - 1, 0);
        }

        ImGui::Spacing();

        if (ImGui::Button("Unpin all particles")) {
            float desiredClothWidth = virtualWidth * coverageWidthPercent;
            float desiredClothHeight = desiredClothWidth * 1.0f;

            size_t newGridWidth = static_cast<size_t>(std::max(2.0f, std::round(desiredClothWidth / newSpacing)));
            size_t newGridHeight = static_cast<size_t>(std::max(2.0f, std::round(desiredClothHeight / newSpacing)));

            for (size_t y = 0; y < newGridHeight; ++y) {
                for (size_t x = 0; x < newGridWidth; ++x) {
                    m_ClothSolver.unpinParticle(x, y);
                }
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Reset cloth")) {
            m_ClothSolver.resize(virtualWidth, virtualHeight, coverageWidthPercent, newSpacing);
        }

        ImGui::End();
    }

    bool is3D() const override { return true; }

    Dimension getDimension() const override { return Dimension::TwoD; }

private:
    ClothSimulation2D m_ClothSolver; // Internal solver
    Renderer2D& m_Renderer;

};