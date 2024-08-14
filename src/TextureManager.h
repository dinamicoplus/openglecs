#pragma once

#include <vector>
#include <string>
#include <map>

class TextureManager
{
public:

	static void create(const std::string textureID, const std::string & texturePath);
	static void free(const std::string textureID);
	static void freeAll();

	static unsigned int get(const std::string textureID);
private:
	static std::map<std::string, unsigned int> m_TextureIDs;
};

