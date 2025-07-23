#pragma once

#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

class Cube
{
public:
	Cube();
	~Cube();

	void create(const Shader& shader, const glm::vec3& pos);
	void setTexture(const std::string& textureID);

	void translate(const Shader& shader, const glm::vec3& deltaPos);
	void rotate(const Shader& shader, float angle, const glm::vec3& axis);
	void scale(const Shader& shader, const glm::vec3& scaleFactor);
	void setColor(const Shader& shader, const glm::vec4& newColor);
	void applyTransformation(const glm::mat4& modelMatrix);

	const glm::vec3 getPosition() const { return m_Pos; }
	const glm::vec3 getRotation() const { return m_Rotation; }
	const glm::vec3 getScale() const { return m_Scale; }

	void render(const Shader& shader);

private:
	glm::vec3 m_Pos{};
	glm::vec3 m_Rotation{};
	glm::vec3 m_Scale{};
	glm::vec4 m_Color{};
	glm::mat4 m_ModelMatrix{};

	unsigned int m_VBO{};
	unsigned int m_VAO{};

	unsigned int m_Texture{};

	glm::vec3 extractEulerAngles();
	
};

