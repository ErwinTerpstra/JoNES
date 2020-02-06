#ifndef _TXROM_H_
#define _TXROM_H_

#include "environment.h"
#include "Mapper.h"

namespace libnes
{
	struct TxROM_Registers
	{
		uint8_t bankSelect;
		uint8_t banks[8];

		uint8_t mirroring;
		uint8_t prgRamProtect;

		uint8_t irqLatch;
		uint8_t irqCounter;
	};

	class TxROM : public Mapper
	{
	private:
		uint8_t* prgRom;
		uint8_t* prgRam;
		uint8_t* chrMem;

		uint8_t* prgRomBanks[4];
		uint8_t* prgRamBank;

		uint8_t* chrBanks[8];

		uint32_t prgRomSize;
		uint32_t prgRamSize;

		TxROM_Registers registers;

		bool usesChrRam;
		bool irqEnabled;
		bool irqReload;

	public:
		TxROM(const CartridgeHeader_iNES& header, const uint8_t* buffer);
		~TxROM();

		uint8_t PeekMain(uint16_t address) const;
		void WriteMain(uint16_t address, uint8_t value);

		uint8_t PeekVideo(uint16_t address) const;
		void WriteVideo(uint16_t address, uint8_t value);

		NametableMirroring GetMirroring() const;

		bool CountScanline();
		
		uint8_t PrgBankCount() const { return (prgRomSize >> 13); }	// 8KB banks

	private:
		void SelectBanks();
	};
}

#endif