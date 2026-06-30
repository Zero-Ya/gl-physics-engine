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

int main()
{
    float screenWidth = (float)SCR_WIDTH;
    float screenHeight = (float)SCR_HEIGHT;
    float maxCircleDiameter = 20.0f;

    // Init app and core systems
    Application app("Physics Engine", SCR_WIDTH, SCR_HEIGHT);
    Time timer;
    Input::init(app.getWindow());

    ProfilerPanel profilerPanel(120);
    float lastTotalFrameTimeMs = 0.0f;

    // Shader
    Shader debugShader("assets/shaders/debug.vert", "assets/shaders/debug.frag");

    DebugDraw debugRenderer(debugShader);

    // Create scene
    Scene scene;
    auto ball1 = scene.createEntity("Ball1", glm::vec2(2.0f, -1.0f), glm::vec2(10.0f, 1.0f));
    auto ball2 = scene.createEntity("Ball2", glm::vec2(-2.0f, 2.0f), glm::vec2(9.0f, 4.0f));
    auto ball3 = scene.createEntity("Ball3", glm::vec2(-1.0f, 3.0f), glm::vec2(-8.0f, 2.0f));
    auto ball4 = scene.createEntity("Ball4", glm::vec2(0.5f, -4.0f), glm::vec2(-7.0f, 5.0f));
    auto ball5 = scene.createEntity("Ball5", glm::vec2(0.0f, 5.0f), glm::vec2(12.0f, 0.0f));

    glm::vec2 gravity(0.0f, -9.81f);

    // Spatial grid init
    SpatialGrid spatialGrid(screenWidth, screenHeight, maxCircleDiameter);
    std::vector<size_t> neighborCellIndices;

    // ImGui init
    ImGuiLayer imGuiLayer(app.getWindow());

    // Render loop
    while (app.isRunning())
    {
        glfwPollEvents();

        // Entire frame time start
        Time::TimePoint frameStart = timer.getCurrentTimePoint();

        // For maintaining aspect ratio
        int dynaWidth, dynaHeight;
        glfwGetFramebufferSize(app.getWindow(), &dynaWidth, &dynaHeight);
        if (dynaHeight == 0) dynaHeight = 1;
        float aspect = (float)dynaWidth / (float)dynaHeight;
        float orthoHeight = 4.5f;
        float orthoWidth = orthoHeight * aspect;
        glm::mat4 projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -1.0f, 1.0f);

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
            scene.createEntity("BallRuntime", clickPos, glm::vec2(0.0f, -10.0f));
        }

        // Spatial grid build
        spatialGrid.clear();
        for (size_t i = 0; i < scene.getEntityCount(); ++i) {
            spatialGrid.insert(i, scene.getEntities()[i].get()->getComponent<Transform2D>()->position); // Bruh
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
            spatialGrid.getNeighborCells(scene.getEntities()[i].get()->getComponent<Transform2D>()->position, neighborCellIndices);

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
        glm::vec2 minWorldBounds(-orthoWidth, -orthoHeight);
        glm::vec2 maxWorldBounds(orthoWidth, orthoHeight);
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
                debugRenderer.drawBox(tf->position, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), projection);
            }
        }

        // ImGui render
        imGuiLayer.beginFrame();
        profilerPanel.onImGuiRender(lastTotalFrameTimeMs, physicsTimeMs, scene.getEntityCount());
        imGuiLayer.endFrame();

        app.swapBuffers();
        Input::postUpdate();

        lastTotalFrameTimeMs = timer.getElapsedTimeMs(frameStart);
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