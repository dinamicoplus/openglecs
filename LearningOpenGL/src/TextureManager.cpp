#include "TextureManager.h"

#include "stb_image.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

constexpr unsigned int TextureManager::MAX_TEXTURES;
unsigned int TextureManager::m_TextureIDs[TextureManager::MAX_TEXTURES] = {0};
unsigned int TextureManager::m_NextTextureID = 0;

unsigned int TextureManager::create(const std::string& texturePath)
{
    if (m_NextTextureID >= MAX_TEXTURES) {
        spdlog::error("Max texture limit reached.");
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        m_TextureIDs[m_NextTextureID] = textureID;
        spdlog::info("Texture {} created at slot {}", textureID, m_NextTextureID);
        stbi_image_free(data);
        return m_NextTextureID++;
    }
    else
    {
        spdlog::error("Failed to load texture at slot {}", m_NextTextureID);
        return 0;
    }
}

void TextureManager::free(unsigned int index)
{
    if (index < m_NextTextureID && m_TextureIDs[index] != 0) {
        glDeleteTextures(1, &m_TextureIDs[index]);
        spdlog::info("Texture at slot {} deleted.", index);
        m_TextureIDs[index] = 0;
    }
}

void TextureManager::freeAll()
{
    for (int i = m_NextTextureID - 1; i >= 0; --i)
    {
        if (m_TextureIDs[i] != 0) {
            glDeleteTextures(1, &m_TextureIDs[i]);
            m_TextureIDs[i] = 0;
        }
    }
    m_NextTextureID = 0;
    spdlog::info("Every texture deleted.");
}

unsigned int TextureManager::get(unsigned int index)
{
    if (index < m_NextTextureID)
        return m_TextureIDs[index];
    return 0;
}
