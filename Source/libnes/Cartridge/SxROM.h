#ifndef _SXROM_H_
#define _SXROM_H_

#include "environment.h"
#include "Mapper.h"

namespace libnes
{
	struct MMC1_Registers
	{
		uint8_t load;
		uint8_t loadWriteCount;

		uint8_t control;
		uint8_t chrBank0;
		uint8_t chrBank1;
		uint8_t prgBank;
	};

	class SxROM : public Mapper
	{
	private:
		uint8_t* prgRom;
		uint8_t* prgRam;
		uint8_t* chrMem;
		
		uint8_t* prgRomBanks[2];
		uint8_t* prgRamBank;

		uint8_t* chrBanks[2];

		uint32_t prgRomSize;
		uint32_t prgRamSize;

		MMC1_Registers registers;

		bool usesChrRam;

	public:
		SxROM(const CartridgeHeader_iNES& header, const uint8_t* buffer);
		~SxROM();

		uint8_t ReadMain(uint16_t address);
		uint8_t PeekMain(uint16_t address) const;
		void WriteMain(uint16_t address, uint8_t value);

		uint8_t ReadVideo(uint16_t address);
		uint8_t PeekVideo(uint16_t address) const;
		void WriteVideo(uint16_t address, uint8_t value);

		bool GetInternalVideoRamA10(uint16_t address) const;
		bool GetInternalVideoRamEnabled(uint16_t address) const;

	private:
		void SelectBanks();
	};
}

#endif