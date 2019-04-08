#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include "environment.h"

#include "CartridgeHeader.h"

namespace libnes
{
	class Cartridge
	{

	private:
		CartridgeHeader_iNES header;

		uint8_t* prgRom;
		uint8_t* chrRom;

		uint32_t prgRomSize;
		uint32_t chrRomSize;

	public:
		Cartridge();

		void Load_iNES(uint8_t* buffer, uint32_t bufferSize);

		uint8_t ReadMain(uint16_t address);
		uint8_t PeekMain(uint16_t address) const;
		void WriteMain(uint16_t address, uint8_t value);
		
		uint8_t ReadVideo(uint16_t address);
		uint8_t PeekVideo(uint16_t address) const;
		void WriteVideo(uint16_t address, uint8_t value);

		bool GetInternalVideoRamA10(uint16_t address) const;
		bool GetInternalVideoRamEnabled(uint16_t address) const;
	};
}

#endif