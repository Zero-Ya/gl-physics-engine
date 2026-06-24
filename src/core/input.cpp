#include "input.h"

#include <glfw/glfw3.h>

void Input::init(GLFWwindow* window) {
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
}

void Input::postUpdate() {
	// At the end of the frame, clone the current states into the previous states map.
	// This gives us the frame-over-frame memory history needed to detect edge transitions!
	m_PreviousKeys = m_CurrentKeys;
	m_PreviousMouseButtons = m_CurrentMouseButtons;
}

// Keyboard state

bool Input::isKeyHeld(int keycode)
{
	return m_CurrentKeys[keycode];
}

bool Input::isKeyPressed(int keycode)
{
	return m_CurrentKeys[keycode] && !m_PreviousKeys[keycode];
}

bool Input::isKeyReleased(int keycode)
{
	return !m_CurrentKeys[keycode] && m_PreviousKeys[keycode];
}

// Mouse state

bool Input::isMouseButtonHeld(int button)
{
	return m_CurrentMouseButtons[button];
}

bool Input::isMouseButtonPressed(int button)
{
	return m_CurrentMouseButtons[button] && !m_PreviousMouseButtons[button];
}

bool Input::isMouseButtonReleased(int button)
{
	return !m_CurrentMouseButtons[button] && m_PreviousMouseButtons[button];
}

// GLFW callbacks

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)   m_CurrentKeys[key] = true;
    if (action == GLFW_RELEASE) m_CurrentKeys[key] = false;
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS)   m_CurrentMouseButtons[button] = true;
    if (action == GLFW_RELEASE) m_CurrentMouseButtons[button] = false;
}

void Input::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    m_MousePos.x = static_cast<float>(xpos);
    m_MousePos.y = static_cast<float>(ypos);
}