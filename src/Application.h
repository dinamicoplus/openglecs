#pragma once

#include "Rectangle.h"
#include "Triangle.h"
#include "Cube.h"
#include "Shader.h"

#include <GLFW/glfw3.h>
#include <vector>

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
	std::vector<Cube> m_Cubes{10};

	glm::vec3 m_CubePositions[10];

	glm::mat4 m_Model;
	glm::mat4 m_View;
	glm::mat4 m_Projection;
	float cameraSpeed = 1.0f;
	

	static constexpr int s_WindowWidth{1920};
	static constexpr int s_WindowHeight{1080};
};

