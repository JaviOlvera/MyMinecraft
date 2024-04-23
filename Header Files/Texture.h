#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vendor/stb_image/stb_image.h"

#include <string>
using namespace std;

class Texture
{
private:
	
	
	
public:

	unsigned int m_RendererID;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
	string m_FilePath;

	Texture();
	Texture(const string& path, unsigned int slot = 0, bool necessary = true);
	~Texture();

	void Bind(unsigned int slot = 0, bool necessary = true) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};