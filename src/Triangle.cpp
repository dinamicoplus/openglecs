#include "Triangle.h"

Triangle::Triangle(float v1x, float v1y, float v2x, float v2y, float v3x, float v3y)
	:
	m_V1x{v1x}, m_V1y{v1y}, m_V2x{ v2x }, m_V2y{ v2y }, m_V3x{ v3x }, m_V3y{ v3y }
{

}

Triangle::~Triangle()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Triangle::create()
{
    float vertices[] = {                                  
        m_V1x,  m_V1y, 0.0f, 1.0f, 0.0f, 1.0f,
        m_V2x,  m_V2y, 0.0f, 0.0f, 1.0f, 0.0f,
        m_V3x,  m_V3y, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    glGenBuffers(1, &m_VBO);
    glGenVertexArrays(1, &m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindVertexArray(m_VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // pos attrib
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Triangle::render()
{
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
