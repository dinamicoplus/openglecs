#pragma once

#include "Rectangle.h"
#include "Triangle.h"
#include "Cube.h"
#include "Shader.h"
#include "Camera.h"

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
	void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	GLFWwindow* m_Window{};
	Shader m_Shader{};
	Camera m_Camera{};
	float m_LastX;
	float m_LastY;
	bool m_FirstMouse;
	float m_Dt{};
	float m_LastFrame{};
	std::vector<Cube> m_Cubes{1000};


	glm::mat4 m_Model;
	glm::mat4 m_View;
	glm::mat4 m_Projection;
	float m_AspectRatio;
	float cameraSpeed = 1.0f;
	

	static constexpr int s_WindowWidth{1000};
	static constexpr int s_WindowHeight{1000};
};

