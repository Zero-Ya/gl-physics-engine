// External includes
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <iostream>

// Core includes
#include "core/application.h"

void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    Application app("Physics Engine", SCR_WIDTH, SCR_HEIGHT);

    // Render loop
    while (app.isRunning())
    {
        // Input
        processInput(app.getWindow());

        // Render
        app.clearScreen(0.2f, 0.3f, 0.3f, 1.0f);

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
