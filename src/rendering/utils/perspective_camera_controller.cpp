#include "perspective_camera_controller.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glfw/glfw3.h>

#include "core/input.h"
#include <algorithm>

PerspectiveCameraController::PerspectiveCameraController(float aspectRatio)
    : m_AspectRatio(aspectRatio),
    m_Camera(45.0f, aspectRatio, 0.1f, 100.0f),
    m_MovementSpeed(5.0f),
    m_MouseSensitivity(0.35f),
    m_Zoom(45.0f)
{}

void PerspectiveCameraController::onUpdate(float ts) {
    // Mouse
    if (Input::isMouseButtonHeld(GLFW_MOUSE_BUTTON_RIGHT)) { // Or lock mouse cursor 
        glm::vec2 mousePos = glm::vec2(Input::getMouseX(), Input::getMouseY());

        if (m_FirstMouse) {
            m_LastMousePosition = mousePos;
            m_FirstMouse = false;
        }

        float xOffset = (mousePos.x - m_LastMousePosition.x) * m_MouseSensitivity;
        float yOffset = (m_LastMousePosition.y - mousePos.y) * m_MouseSensitivity; // Inverted Y

        m_LastMousePosition = mousePos;

        m_Yaw += xOffset;
        m_Pitch += yOffset;

        // Clamp Pitch to prevent camera flipping upside down (Gimbal Lock protection)
        m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

        m_Camera.setRotation(m_Pitch, m_Yaw);

    }
    else {
        m_FirstMouse = true; // Reset initial offset when releasing right-click
    }

    // WASD movement
    float velocity = m_MovementSpeed * ts;

    if (Input::isKeyHeld(GLFW_KEY_W))
        m_CameraPosition += m_Camera.getFrontVector() * velocity;
    if (Input::isKeyHeld(GLFW_KEY_S))
        m_CameraPosition -= m_Camera.getFrontVector() * velocity;

    if (Input::isKeyHeld(GLFW_KEY_A))
        m_CameraPosition -= m_Camera.getRightVector() * velocity;
    if (Input::isKeyHeld(GLFW_KEY_D))
        m_CameraPosition += m_Camera.getRightVector() * velocity;

    // Vertical movement
    if (Input::isKeyHeld(GLFW_KEY_E))
        m_CameraPosition.y += velocity;
    if (Input::isKeyHeld(GLFW_KEY_Q))
        m_CameraPosition.y -= velocity;

    m_Camera.setPosition(m_CameraPosition);

    // Add zoom later
}
