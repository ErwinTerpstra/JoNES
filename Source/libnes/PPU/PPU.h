#ifndef _PPU_H_
#define _PPU_H_

#include "environment.h"

namespace libnes
{
	struct Sprite
	{
		uint8_t y;
		uint8_t tileIdx;
		uint8_t attributes;
		uint8_t x;
	};

	class PPU
	{
	private:
		int16_t scanline;

	public:
		PPU();
	};
}

#endif