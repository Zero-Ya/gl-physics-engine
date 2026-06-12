// External includes
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

// Core includes
#include "core/application.h"

// UI
#include "ui/imgui_layer.h"

// Rendering
#include "rendering/utils/shader.h"

void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // Init app
    Application app("Physics Engine", SCR_WIDTH, SCR_HEIGHT);

    // Shaders and buffers
    Shader basicShader("assets/shaders/2d_basic_shader.vs", "assets/shaders/2d_basic_shader.fs");

    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ImGui init
    ImGuiLayer imGuiLayer(app.getWindow());

    // Uniform data
    float size = 1.0f;
    glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    basicShader.use();
    basicShader.setFloat("size", size);
    basicShader.setVec4("color", color);

    // Render loop
    while (app.isRunning())
    {
        // Input
        processInput(app.getWindow());

        // Render
        app.clearScreen(0.2f, 0.3f, 0.3f, 1.0f);

        // Render the triangle
        basicShader.use();
        basicShader.setFloat("size", size);
        basicShader.setVec4("color", color);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // ImGui render
        imGuiLayer.beginFrame();
        imGuiLayer.render(size, color);
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
