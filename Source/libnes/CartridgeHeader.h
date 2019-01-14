#ifndef _CARTRIDGE_HEADER_H_
#define _CARTRIDGE_HEADER_H_

#include "environment.h"

namespace libnes
{
	struct CartridgeHeader_iNES
	{
		char format[4];
		uint8_t prgRomSize;
		uint8_t chrRomSize;
		uint8_t flags6;
		uint8_t flags7;
		uint8_t flags8;
		uint8_t flags9;
		uint8_t flags10;
		uint8_t padding[5];

		uint8_t GetMapper() const
		{
			return (flags7 & 0xF0) | ((flags6 & 0xF0) >> 4);
		}

		uint32_t GetPrgRomSizeInBytes() const
		{
			return prgRomSize << 14;
		}

		uint32_t GetChrRomSizeInBytes() const
		{
			return chrRomSize << 13;
		}

		bool IsNES20() const
		{
			return (flags7 & 0x0C) == 0x08;
		}
	};
}

#endif