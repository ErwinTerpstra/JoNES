#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include "environment.h"

namespace libnes
{
	class Cartridge
	{

	private:
		uint8_t* prgRom;
		uint8_t* chrRom;

		uint32_t prgRomSize;
		uint32_t chrRomSize;

	public:
		Cartridge();

		void Load_iNES(uint8_t* buffer, uint32_t bufferSize);

		uint8_t Read(uint16_t address) const;
		void Write(uint16_t address, uint8_t value);
	};
}

#endif