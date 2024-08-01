#pragma once

#include "Rectangle.h"
#include "Triangle.h"
#include "Cube.h"
#include "Shader.h"

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

private:
	GLFWwindow* m_Window{};
	Shader m_Shader{};
	Rect m_Rect{ 0.0f, 0.0f, 1.0f, 1.0f };
	Cube m_Cube{};
	

	static constexpr int s_WindowWidth{1920};
	static constexpr int s_WindowHeight{1080};
};

