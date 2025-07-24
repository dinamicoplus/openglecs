#pragma once

#include <vector>
#include <string>
#include <map>

class TextureManager
{
public:
    static unsigned int create(const std::string& texturePath);
    static void free(unsigned int textureID);
    static void freeAll();
    static unsigned int get(unsigned int index);

private:
    static constexpr unsigned int MAX_TEXTURES = 32; // Ajusta según tus necesidades
    static unsigned int m_TextureIDs[MAX_TEXTURES];
    static unsigned int m_NextTextureID;
};

