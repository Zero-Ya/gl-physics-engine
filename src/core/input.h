#pragma once

#include <glm/glm.hpp>

#include <unordered_map>

struct GLFWwindow;

class Input {
public:
	static void init(GLFWwindow* window);

	static void postUpdate();

	// Keyboard state
	static bool isKeyHeld(int keycode);
	static bool isKeyPressed(int keycode);
	static bool isKeyReleased(int keycode);

	// Mouse state
	static bool isMouseButtonHeld(int button);
	static bool isMouseButtonPressed(int button);
	static bool isMouseButtonReleased(int button);
	static glm::vec2 getMousePosition() { return m_MousePos; }
	static float getMouseX() { return m_MousePos.x; }
	static float getMouseY() { return m_MousePos.y; }

private:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

	inline static std::unordered_map<int, bool> m_CurrentKeys;
	inline static std::unordered_map<int, bool> m_PreviousKeys;

	inline static std::unordered_map<int, bool> m_CurrentMouseButtons;
	inline static std::unordered_map<int, bool> m_PreviousMouseButtons;

	inline static glm::vec2 m_MousePos{ 0.0f, 0.0f };
};