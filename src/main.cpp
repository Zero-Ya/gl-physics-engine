// External includes
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <vector>

// Core includes
#include "core/application.h"
#include "core/time.h"

// UI
#include "ui/imgui_layer.h"

// Rendering
#include "rendering/utils/shader.h"
#include "rendering/2D/debug_draw.h"

// Physics
#include "physics/2D/integrator2d.h"

// Components
#include "components/2D/rigidbody2d.h"
#include "components/2D/transform2d.h"

// Scene
#include "scene/game_object.h"


void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // Init app
    Application app("Physics Engine", SCR_WIDTH, SCR_HEIGHT);
    Time timer;

    // Shader
    Shader debugShader("assets/shaders/debug.vert", "assets/shaders/debug.frag");

    DebugDraw debugRenderer(debugShader);

    glm::mat4 projection = glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);

    std::vector<std::unique_ptr<GameObject>> entities;
    std::unique_ptr box = std::make_unique<GameObject>(1, "Box");

    box->addComponent<Transform2D>(glm::vec2(0.0f, 3.0f));

    auto* rb = box->addComponent<RigidBody2D>(1.0f);
    rb->velocity = glm::vec2(1.5f, 0.0f);

    entities.push_back(std::move(box));

    glm::vec2 gravity(0.0f, -9.81f);

    // Buffers and bindings

    // ImGui init
    ImGuiLayer imGuiLayer(app.getWindow());

    // Uniform data

    // Render loop
    while (app.isRunning())
    {
        timer.update();
        float dt = timer.getDeltaTime();

        // To prevent massive physics jumps during lag spikes
        if (dt > 0.1f) dt = 0.1f;

        // Input
        processInput(app.getWindow());

        // Physics phase
        for (auto& obj : entities) {
            Integrator2D::integrate(*obj, dt, gravity);
        }

        // Render the scene
        app.clearScreen(0.2f, 0.3f, 0.3f, 1.0f);

        for (auto& obj : entities) {
            auto* tf = obj->getComponent<Transform2D>();
            if (tf) {
                debugRenderer.drawBox(tf->position, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), projection);
            }
        }

        // ImGui render
        imGuiLayer.beginFrame();
        //imGuiLayer.render(size, color);
        imGuiLayer.endFrame();

        // GLFW: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        app.swapBuffers();
        glfwPollEvents();
    }

    return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
