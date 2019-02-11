#include "stdafx.h"

#include "Texture.h"

using namespace JoNES;

Texture::Texture(int width, int height, bool alpha) : width(width), height(height), alpha(alpha)
{
	GL(glGenTextures(1, &handle));
}

Texture::~Texture()
{
	if (handle)
	{
		glDeleteTextures(1, &handle);
		handle = 0;
	}
}

void Texture::Load(const uint8_t* buffer)
{
	Bind();

	// Setup sampler state
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format;
	GLint internalFormat;

	if (alpha)
	{
		format = GL_RGBA;
		internalFormat = GL_RGBA;
	}
	else
	{
		format = GL_RGB;
		internalFormat = GL_RGB;
	}

	// Upload source image
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	GL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, buffer));

	ClearBoundTexture();
}

void Texture::Bind()
{
	GL(glBindTexture(GL_TEXTURE_2D, handle));
}

void Texture::ClearBoundTexture()
{
	GL(glBindTexture(GL_TEXTURE_2D, 0));
}