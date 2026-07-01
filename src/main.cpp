// External includes
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <random>

// Core includes
#include "core/application.h"
#include "core/time.h"
#include "core/input.h"

// UI
#include "ui/imgui_layer.h"
#include "ui/panels/profiler_panel.h"

// Rendering
#include "rendering/utils/shader.h"
#include "rendering/2D/debug_draw.h"

// Physics
#include "physics/2D/integrator2d.h"
#include "physics/2D/boundary_solver2d.h"
#include "physics/2D/collision_solver2d.h"
#include "physics/2D/spatial_grid.h"

// Components
#include "components/2D/rigidbody2d.h"
#include "components/2D/transform2d.h"

// Scene
#include "scene/game_object.h"
#include "scene/scene.h"

void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec2 screenToWorld(glm::vec2 pos, int windowWidth, int windowHeight, float orthoHeight);
void calcAspectRatio(int& dynaWidth,
                     int& dynaHeight,
                     float& orthoWidth,
                     float& orthoHeight,
                     glm::mat4& projection);

int main()
{
    float maxCircleDiameter = 0.5f;

    // Init app and core systems
    Application app("Physics Engine", SCR_WIDTH, SCR_HEIGHT);
    Time timer;
    Input::init(app.getWindow());

    ProfilerPanel profilerPanel(120);
    float lastTotalFrameTimeMs = 0.0f;

    // Shader
    Shader debugShader("assets/shaders/debug.vert", "assets/shaders/debug.frag");

    DebugDraw debugRenderer(debugShader);

    // Random num init... I'm not completely sure how this works
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distr(-10, 10);

    // Create scene
    Scene scene;
    for (int i = 0; i < 500; ++i) {
        scene.createEntity("Ball", glm::vec2(0.0f, 0.0f), glm::vec2(static_cast<float>(distr(gen)), static_cast<float>(distr(gen))), 0.1f);
    }

    glm::vec2 gravity(0.0f, 0.0f); // Zero gravity
    //glm::vec2 gravity(0.0f, -9.81f);

    // Track window size, view size and projection stuff
    int dynaWidth, dynaHeight;
    glfwGetFramebufferSize(app.getWindow(), &dynaWidth, &dynaHeight);
    int lastWidth = dynaWidth;
    int lastHeight = dynaHeight;

    float orthoWidth{};
    float orthoHeight{};
    glm::mat4 projection{};
    calcAspectRatio(dynaWidth, dynaHeight, orthoWidth, orthoHeight, projection);

    // ImGui init
    ImGuiLayer imGuiLayer(app.getWindow());

    // Spatial grid init
    SpatialGrid spatialGrid(orthoWidth * 2.0f, orthoHeight * 2.0f, maxCircleDiameter);
    std::vector<size_t> neighborCellIndices;
    neighborCellIndices.reserve(9);

    // Boundary size
    glm::vec2 minWorldBounds(-orthoWidth, -orthoHeight);
    glm::vec2 maxWorldBounds(orthoWidth, orthoHeight);

    // Render loop
    while (app.isRunning())
    {
        glfwPollEvents();

        // Entire frame time start
        Time::TimePoint frameStart = timer.getCurrentTimePoint();

        // For maintaining aspect ratio
        glfwGetFramebufferSize(app.getWindow(), &dynaWidth, &dynaHeight);
        if (dynaHeight == 0) dynaHeight = 1;
        if (dynaWidth != lastWidth || dynaHeight != lastHeight) {
            lastWidth = dynaWidth;
            lastHeight = dynaHeight;

            calcAspectRatio(dynaWidth, dynaHeight, orthoWidth, orthoHeight, projection);
            spatialGrid.resize(orthoWidth * 2.0f, orthoHeight * 2.0f);

            minWorldBounds = glm::vec2(-orthoWidth, -orthoHeight);
            maxWorldBounds = glm::vec2(orthoWidth, orthoHeight);
        }

        timer.update();
        float dt = timer.getDeltaTime();

        // To prevent massive physics jumps during lag spikes
        if (dt > 0.1f) dt = 0.1f;

        // Inputs
        processInput(app.getWindow());

        if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
            app.close();
        }

        if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec2 clickPos = screenToWorld(Input::getMousePosition(), dynaWidth, dynaHeight, orthoHeight);
            scene.createEntity("BallRuntime", clickPos, glm::vec2(0.0f, -10.0f), 0.1f);
        }

        // Spatial grid build
        spatialGrid.clear();
        for (size_t i = 0; i < scene.getEntityCount(); ++i) {
            spatialGrid.insert(i, scene.getEntities()[i].get()->getComponent<Transform2D>()->position, orthoWidth, orthoHeight); // Bruh
        }

        // Physics time start
        Time::TimePoint physicsStart = timer.getCurrentTimePoint();
        // Movement
        for (auto& obj : scene.getEntities()) {
            Integrator2D::integrate(obj.get(), dt, gravity);
        }

        // Old version of obj-obj collision
        //for (size_t i = 0; i < scene.getEntityCount(); ++i) {
        //    for (size_t j = i + 1; j < scene.getEntityCount(); ++j) {
        //        CollisionSolver2D::resolveCircleCollision(scene.getEntities()[i].get(), scene.getEntities()[j].get());
        //    }
        //}

        // Object-object collision
        for (size_t i = 0; i < scene.getEntityCount(); ++i) {
            spatialGrid.getNeighborCells(scene.getEntities()[i].get()->getComponent<Transform2D>()->position, neighborCellIndices, orthoWidth, orthoHeight);

            for (size_t cellIdx : neighborCellIndices) {
                const auto& cell = spatialGrid.getCell(cellIdx);

                for (size_t neighborIndex : cell.entityIndices) {
                    // Double-Check Protection: 
                    // 1. Don't check an entity against itself
                    // 2. Only resolve if i < neighborIndex to enforce one check per pair
                    if (i >= neighborIndex) continue;
                    CollisionSolver2D::resolveCircleCollision(scene.getEntities()[i].get(), scene.getEntities()[neighborIndex].get());
                }
            }
        }

        // Boundary collision
        for (auto& obj : scene.getEntities()) {
            BoundarySolver2D::resolveCollision(obj.get(), minWorldBounds, maxWorldBounds);
        }

        // Stop time for physics engine
        float physicsTimeMs = timer.getElapsedTimeMs(physicsStart);

        // Render the scene
        app.clearScreen(0.2f, 0.3f, 0.3f, 1.0f);

        for (auto& obj : scene.getEntities()) {
            auto* tf = obj->getComponent<Transform2D>();
            if (tf) {
                debugRenderer.drawBox(tf->position, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), projection, tf->radius);
            }
        }

        // ImGui render
        imGuiLayer.beginFrame();
        if (profilerPanel.isDebugGridEnabled()) {
            spatialGrid.drawDebugGrid(static_cast<float>(dynaWidth), static_cast<float>(dynaHeight), orthoWidth, orthoHeight);
        }

        profilerPanel.onImGuiRender(lastTotalFrameTimeMs, physicsTimeMs, scene.getEntityCount());
        imGuiLayer.endFrame();

        app.swapBuffers();
        Input::postUpdate();

        lastTotalFrameTimeMs = timer.getElapsedTimeMs(frameStart);
        lastWidth = dynaWidth;
        lastHeight = dynaHeight;
    }

    return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Map pixels to a normalized device coordinate system (-1.0 to 1.0)
glm::vec2 screenToWorld(glm::vec2 pos, int windowWidth, int windowHeight, float orthoHeight) {
    float aspect = (float)windowWidth / (float)windowHeight;
    float orthoWidth = orthoHeight * aspect;

    float ndcX = (2.0f * (float)pos.x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * (float)pos.y) / windowHeight; // Flip Y axis

    // Multiply by current camera viewing bounds
    return glm::vec2(ndcX * orthoWidth, ndcY * orthoHeight);
}

void calcAspectRatio(int& dynaWidth,
                     int& dynaHeight,
                     float& orthoWidth,
                     float& orthoHeight,
                     glm::mat4& projection)
{ 
    float aspect = (float)dynaWidth / (float)dynaHeight;
    orthoHeight = 4.5f;
    orthoWidth = orthoHeight * aspect;
    projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -1.0f, 1.0f);
}