#include "Rectangle.h"

#include "stb_image.h"

#include <spdlog/spdlog.h>

Rect::Rect(float posX, float posY, float width, float height)
	:
	m_PosX{posX}, m_PosY{posY}, m_Width{width}, m_Height{height}
{
}

Rect::~Rect()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Rect::create()
{
    // Loading texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("resources/container.jpg", &width, &height, &nrChannels, 0);

    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        spdlog::error("Failed to load texture");
    }



    stbi_image_free(data);

    float vertices[] = {
        // positions                                        // color                 // texture
        m_Width / 2.0f + m_PosX,  m_Height / 2.0f + m_PosY, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
        m_Width / 2.0f + m_PosX, -m_Height / 2.0f + m_PosY, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -m_Width / 2.0f + m_PosX, -m_Height / 2.0f + m_PosY, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -m_Width / 2.0f + m_PosX,  m_Height / 2.0f + m_PosY, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // top left 
    };

    unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // pos attrib
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attrib
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attrib
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Rect::render()
{
    glBindVertexArray(m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}
