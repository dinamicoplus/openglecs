#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

static const float YAW = -90.0f;
static const float PITCH = 0.0f;
static const float SPEED = 20.5f;
static const float SENSIVITY = 0.1f;
static const float ZOOM = 45.0f;


class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
		float yaw = YAW, float pitch = PITCH);

	Camera(float posX, float posY, float posZ,
		float upX, float upY, float upZ, 
		float yaw, float pitch);

	glm::mat4 GetViewMatrix();

	void ProcessKeyboard(Camera_Movement direction, float dt);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

	// Getters
	float GetZoom() const { return m_Zoom; }

private:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	float m_Yaw;
	float m_Pitch;

	float m_MovementSpeed;
	float m_MouseSensivity;
	float m_Zoom;


	void updateCameraVectors();
};

