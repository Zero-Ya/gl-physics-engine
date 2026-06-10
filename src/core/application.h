#pragma once

struct GLFWwindow;

class Application {
public:
	Application(const char* title, int width, int height);
	~Application();

	// Controls
	bool isRunning() const;
	void clearScreen(float r, float g, float b, float a) const;
	void swapBuffers() const;

	// Window getter
	GLFWwindow* getWindow() const { return m_Window; }

	// Dimensions getter
	int getWidth() const { return m_Width; }
	int getHeight() const { return m_Height; }

private:
	void initWindow(const char* title);

	GLFWwindow* m_Window;
	int m_Width;
	int m_Height;
};