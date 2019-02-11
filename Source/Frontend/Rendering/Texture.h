#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "libgl.h"

namespace JoNES
{
	class Texture
	{
	private:
		GLuint handle;

		int width, height;
		bool alpha;

	public:
		Texture(int width, int height, bool alpha);
		~Texture();

		void Bind();

		void Load(const uint8_t* buffer);

		bool IsValid() const { return handle != 0; }
		int Width() const { return width; }
		int Height() const { return height; }

		static void ClearBoundTexture();
	};
}

#endif