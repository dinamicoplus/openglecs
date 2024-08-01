#pragma once

#include <glad/glad.h>

class Cube
{
public:
	Cube();
	~Cube();

	void create();
	void render();

private:
	unsigned int m_VBO{};
	unsigned int m_VAO{};

	unsigned int m_Texture1{};
	unsigned int m_Texture2{};
};

