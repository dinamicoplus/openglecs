#include "TextureManager.h"

#include "stb_image.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>

std::map<std::string, unsigned int> TextureManager::m_TextureIDs;

void TextureManager::create(const std::string textureID, const std::string& texturePath)
{
    // Loading texture
    glGenTextures(1, &m_TextureIDs[textureID]);
    glBindTexture(GL_TEXTURE_2D, m_TextureIDs[textureID]);
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
        spdlog::info("Texture named '{}' created.", textureID);
    }
    else
    {
        spdlog::error("Failed to load texture named {}", textureID);
    }
}

void TextureManager::free(const std::string textureID)
{
    glDeleteTextures(1, &m_TextureIDs[textureID]);
    spdlog::info("Texture named '{}' deleted.", textureID);
}

void TextureManager::freeAll()
{
    for (auto const& [key, val] : m_TextureIDs)
    {
        glDeleteTextures(1, &val);
    }
    spdlog::info("Every texture deleted.");
}

unsigned int TextureManager::get(const std::string textureID)
{
    return m_TextureIDs[textureID];
}
