#pragma once

#include "Rectangle.h"
#include <GLFW/glfw3.h>

class Application
{
public:
	Application();
	~Application();

	void updateInput();
	void update();
	void render();

	void close();

	bool isRunning() const { return !glfwWindowShouldClose(m_Window); }

private:
	void initGLFW();
	void initGLAD();

	// Callbacks
	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	void compileShaders();

private:
	GLFWwindow* m_Window{};
	Rect rect1{ 0.4f, 0.4f, 0.5f, 1.0f };
	Rect rect2{ -0.4f, -0.4f, 1.f, 0.5f };
	unsigned int m_ShaderProgram{};

	static constexpr int s_WindowWidth{1920};
	static constexpr int s_WindowHeight{1080};
};

