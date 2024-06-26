#include "Texture.h"
#include <iostream>
using namespace std;

Texture::Texture()
{

}

Texture::Texture(const string& path, unsigned int slot, bool necessary) : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	if (necessary)
	{
		stbi_set_flip_vertically_on_load(1);
		m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

		glActiveTexture(GL_TEXTURE0 + slot); //a�adido por mi cuenta
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_LocalBuffer)
		{
			stbi_image_free(m_LocalBuffer);
		}
	}
}

Texture::~Texture()
{
	//glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot, bool necessary) const
{
	if (necessary)
	{
		//Selecciona el slot de memoria donde vamos a acceder a la textura
		glActiveTexture(GL_TEXTURE0 + slot);

		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
	
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}