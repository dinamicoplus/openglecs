#pragma once

#include <glad/glad.h>

class Triangle
{
public:
	Triangle(float v1x, float v1y, 
		float v2x, float v2y,
		float v3x, float v3y);
	~Triangle();

	void create();
	void render();

private:
	float m_V1x{};
	float m_V1y{};
	float m_V2x{};
	float m_V2y{};
	float m_V3x{};
	float m_V3y{};
	unsigned int m_VBO{};
	unsigned int m_VAO{};
};

