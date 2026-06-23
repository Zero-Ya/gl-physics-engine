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
#include "physics/2D/boundary_solver2d.h"
#include "physics/2D/collision_solver2d.h"

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

    // Hardcoded for now...
    std::vector<std::unique_ptr<GameObject>> entities;
    std::unique_ptr box1 = std::make_unique<GameObject>(1, "Box1");
    std::unique_ptr box2 = std::make_unique<GameObject>(2, "Box2");
    std::unique_ptr box3 = std::make_unique<GameObject>(3, "Box3");
    std::unique_ptr box4 = std::make_unique<GameObject>(4, "Box4");
    std::unique_ptr box5 = std::make_unique<GameObject>(5, "Box5");

    box1->addComponent<Transform2D>(glm::vec2(0.0f, 1.0f));
    box2->addComponent<Transform2D>(glm::vec2(0.0f, 2.0f));
    box3->addComponent<Transform2D>(glm::vec2(0.0f, 3.0f));
    box4->addComponent<Transform2D>(glm::vec2(0.0f, 4.0f));
    box5->addComponent<Transform2D>(glm::vec2(0.0f, 5.0f));

    auto* rb1 = box1->addComponent<RigidBody2D>(1.0f);
    rb1->velocity = glm::vec2(10.0f, 0.0f);

    auto* rb2 = box2->addComponent<RigidBody2D>(1.0f);
    rb2->velocity = glm::vec2(9.0f, 0.0f);

    auto* rb3 = box3->addComponent<RigidBody2D>(1.0f);
    rb2->velocity = glm::vec2(8.0f, 0.0f);

    auto* rb4 = box4->addComponent<RigidBody2D>(1.0f);
    rb4->velocity = glm::vec2(12.0f, 0.0f);

    auto* rb5 = box5->addComponent<RigidBody2D>(1.0f);
    rb5->velocity = glm::vec2(7.5f, 0.0f);

    entities.push_back(std::move(box1));
    entities.push_back(std::move(box2));
    entities.push_back(std::move(box3));
    entities.push_back(std::move(box4));
    entities.push_back(std::move(box5));

    glm::vec2 gravity(0.0f, -9.81f);

    // ImGui init
    ImGuiLayer imGuiLayer(app.getWindow());

    // Render loop
    while (app.isRunning())
    {
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

        // Input
        processInput(app.getWindow());

        // Movement
        for (auto& obj : entities) {
            Integrator2D::integrate(obj.get(), dt, gravity);
        }

        // Object-object collision
        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                CollisionSolver2D::resolveCircleCollision(entities[i].get(), entities[j].get());
            }
        }

        glm::vec2 minWorldBounds(-orthoWidth, -orthoHeight);
        glm::vec2 maxWorldBounds(orthoWidth, orthoHeight);

        // Boundary collision
        for (auto& obj : entities) {
            BoundarySolver2D::resolveCollision(obj.get(), minWorldBounds, maxWorldBounds);
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
