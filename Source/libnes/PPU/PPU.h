#ifndef _PPU_H_
#define _PPU_H_

#include "environment.h"
#include "nes.h"

namespace libnes
{
	struct Sprite
	{
		uint8_t y;
		uint8_t tileIdx;
		uint8_t attributes;
		uint8_t x;
	};

	struct Device;
	
	class PPU
	{
	private:
		Device* device;
		uint8_t* frameBuffer;

		uint8_t* oam;

		uint64_t cycles;
		uint16_t scanline;

		uint8_t statusRegister;
		uint8_t controlBits;
		uint8_t maskBits;
		uint8_t oamAddress;
		uint8_t scrollX;
		uint8_t scrollY;
		uint16_t addressRegister;
		bool writeLSB;

		bool nmiState;

	public:
		PPU(Device* device, uint8_t* frameBuffer);
		~PPU();

		void Reset();

		void Tick();

		uint8_t ReadRegister(uint16_t address);
		void WriteRegister(uint16_t address, uint8_t value);

		void PerformOAMDMA(uint8_t addressMSB);

		void DecodeColor(uint8_t color, uint8_t* buffer) const;
		void DecodePatternTable(uint16_t address, uint8_t* buffer);

		uint64_t MasterClockCycles() const
		{
			return cycles * NES_NTSC_PPU_CLOCK_DIVIDER;
		}
	private:

		void IncrementAddress();
		void DrawScanline();

		void DecodeTileRow(uint16_t address, uint8_t column, uint8_t row, uint8_t y, uint8_t* buffer);
		void DecodeTileRow(uint16_t address, uint8_t y, uint8_t* buffer);
	};
}

#endif