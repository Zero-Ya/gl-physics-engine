// External includes
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>

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
#include "rendering/utils/renderer2d.h"

// Physics engine
#include "simulations/collision_simulation2d.h"

void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec2 screenToWorld(glm::vec2 pos, int windowWidth, int windowHeight, float aspectedHeight);

void calcAspectRatio(int& dynaWidth,
                     int& dynaHeight,
                     float& aspectedWidth,
                     float& aspectedHeight,
                     glm::mat4& projection);

int main()
{
    // Init app and core systems
    Application app("Physics Engine", SCR_WIDTH, SCR_HEIGHT);
    Time timer;
    Input::init(app.getWindow());

    ProfilerPanel profilerPanel(120);
    float lastTotalFrameTimeMs = 0.0f;

    // Track window size, view size and projection stuff
    int dynaWidth, dynaHeight;
    glfwGetFramebufferSize(app.getWindow(), &dynaWidth, &dynaHeight);
    int lastWidth = dynaWidth;
    int lastHeight = dynaHeight;

    float aspectedWidth {};
    float aspectedHeight {};
    glm::mat4 projection {};
    calcAspectRatio(dynaWidth, dynaHeight, aspectedWidth, aspectedHeight, projection);

    // ImGui init
    ImGuiLayer imGuiLayer(app.getWindow());

    // Renderer init
    Renderer2D renderer;
    renderer.init();

    // Physics engine init
    CollisionSimulation2D collisionEngine;
    collisionEngine.init(500, aspectedWidth, aspectedHeight);

    collisionEngine.setWorldBounds(glm::vec2(-aspectedWidth, -aspectedHeight), glm::vec2(aspectedWidth, aspectedHeight));
    renderer.setOrthographicProjection(-aspectedWidth, aspectedWidth, -aspectedHeight, aspectedHeight);

    // Render loop
    while (app.isRunning())
    {
        glfwPollEvents();

        // Entire frame time start
        Time::TimePoint frameStart = timer.getCurrentTimePoint();

        timer.update();
        float dt = timer.getDeltaTime();
        // To prevent massive physics jumps during lag spikes
        if (dt > 0.1f) dt = 0.1f;

        // Terrible window resize event callback
        glfwGetFramebufferSize(app.getWindow(), &dynaWidth, &dynaHeight);
        if (dynaHeight == 0) dynaHeight = 1;
        if (dynaWidth != lastWidth || dynaHeight != lastHeight) {
            lastWidth = dynaWidth;
            lastHeight = dynaHeight;

            calcAspectRatio(dynaWidth, dynaHeight, aspectedWidth, aspectedHeight, projection);

            renderer.setOrthographicProjection(-aspectedWidth, aspectedWidth, -aspectedHeight, aspectedHeight);
            collisionEngine.updateSpatialGridAndWorldBounds(aspectedWidth, aspectedHeight);
        }

        // Inputs
        processInput(app.getWindow());

        if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
            app.close();
        }

        if (Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec2 clickPos = screenToWorld(Input::getMousePosition(), dynaWidth, dynaHeight, aspectedHeight);
            collisionEngine.spawnCircle(clickPos, glm::vec2(0.0f, -10.0f), 0.1f);
        }



        // Engine start
        // Physics engine update
        collisionEngine.update(dt, aspectedWidth, aspectedHeight);

        // Render scene
        app.clearScreen(0.2f, 0.3f, 0.3f, 1.0f);
        renderer.submit(collisionEngine.getCircles());



        // ImGui render /////////////
        imGuiLayer.beginFrame();
        if (profilerPanel.isDebugGridEnabled()) {
            collisionEngine.getSpatialGrid()->drawDebugGrid(static_cast<float>(dynaWidth), static_cast<float>(dynaHeight), aspectedWidth, aspectedHeight);
        }
        profilerPanel.onImGuiRender(lastTotalFrameTimeMs, collisionEngine.getPhysicsTimeMs(), collisionEngine.getEntityCount());
        imGuiLayer.endFrame();
        /////////////////////////////
        


        // Stop total time for one frame and update last width, height of the frame
        lastTotalFrameTimeMs = timer.getElapsedTimeMs(frameStart);
        lastWidth = dynaWidth;
        lastHeight = dynaHeight;

        app.swapBuffers();
        Input::postUpdate();
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
glm::vec2 screenToWorld(glm::vec2 pos, int windowWidth, int windowHeight, float aspectedHeight) {
    float aspect = (float)windowWidth / (float)windowHeight;
    float aspectedWidth = aspectedHeight * aspect;

    float ndcX = (2.0f * (float)pos.x) / windowWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * (float)pos.y) / windowHeight; // Flip Y axis

    // Multiply by current camera viewing bounds
    return glm::vec2(ndcX * aspectedWidth, ndcY * aspectedHeight);
}

void calcAspectRatio(int& dynaWidth,
                     int& dynaHeight,
                     float& aspectedWidth,
                     float& aspectedHeight,
                     glm::mat4& projection)
{
    float aspect = (float)dynaWidth / (float)dynaHeight;
    aspectedHeight = 4.5f;
    aspectedWidth = aspectedHeight * aspect;
    projection = glm::ortho(-aspectedWidth, aspectedWidth, -aspectedHeight, aspectedHeight, -1.0f, 1.0f);
}