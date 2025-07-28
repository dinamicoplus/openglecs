#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
	float x, y, z;
	float r, g, b;
	float u, v;
	float s, t, w;
};

struct TexturedModelComponent
{
		glm::vec3 m_Rotation{};
		glm::vec3 m_Scale{};
		glm::mat4 m_ModelMatrix{};
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		GLuint m_VBO, m_VAO;
		unsigned int m_TextureID{0}; // Default to 0 (no texture)
		
};

