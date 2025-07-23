#pragma once

#include <glad/glad.h>

class Rect
{
public:
	Rect(float posX, float posY, float width, float height);
	~Rect();

	void create();
	void render();

private:
	float m_PosX{};
	float m_PosY{};
	float m_Width{};
	float m_Height{};
	unsigned int m_VBO{};
	unsigned int m_VAO{};
	unsigned int m_EBO{};
	unsigned int m_Texture1{};
	unsigned int m_Texture2{};
};

