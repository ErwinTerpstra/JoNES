#ifndef _NROM_H_
#define _NROM_H_

#include "Mapper.h"

namespace libnes
{
	struct CartridgeHeader_iNES;

	class NROM : public Mapper
	{
	private:

		uint8_t* prgRom;
		uint8_t* chrRom;

		uint32_t prgRomSize;
		uint32_t chrRomSize;

	public:
		NROM(const CartridgeHeader_iNES& header, const uint8_t* buffer);
		~NROM();

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