#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct GLFWwindow;

class ImGuiLayer {
public:
	ImGuiLayer(GLFWwindow* window);
	~ImGuiLayer();

	// Should only be used after OpenGL has finished rendering in the main loop
	void beginFrame();
	void renderDemo(float& size, glm::vec4& color);
	void endFrame();
private:
	void init(GLFWwindow* window);
	void shutdown();
};