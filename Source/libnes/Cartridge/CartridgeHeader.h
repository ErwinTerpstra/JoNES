#ifndef _CARTRIDGE_HEADER_H_
#define _CARTRIDGE_HEADER_H_

#include "environment.h"
#include "util.h"

namespace libnes
{
	enum NametableMirroring
	{
		MIRROR_NONE,
		MIRROR_HORIZONTAL,
		MIRROR_VERTICAL,
		MIRROR_ONE_BANK_LOWER,
		MIRROR_ONE_BANK_UPPER
	};

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

		bool IsNES20() const
		{
			return (flags7 & 0x0C) == 0x08;
		}

		bool HasPrgRam() const
		{
			return !TEST_BIT(flags10, 4);
		}

		uint8_t GetMapper() const
		{
			return (flags7 & 0xF0) | ((flags6 & 0xF0) >> 4);
		}

		uint32_t GetPrgRomSizeInBytes() const
		{
			return prgRomSize << 14;
		}

		uint32_t GetPrgRamSizeInBytes() const
		{
			if (HasPrgRam() && flags8 == 0)
				return 0x8000;	// use 32KB for compatibility purposes
			else
				return flags8 << 13;
		}

		uint32_t GetChrRomSizeInBytes() const
		{
			return chrRomSize << 13;
		}

		NametableMirroring GetMirroring() const
		{
			if (DisableMirroring())
				return MIRROR_NONE;

			if (TEST_BIT(flags6, 0))
				return MIRROR_VERTICAL;
			else
				return MIRROR_HORIZONTAL;
		}

		bool DisableMirroring() const
		{
			return TEST_BIT(flags6, 3);
		}
	};
}

#endif