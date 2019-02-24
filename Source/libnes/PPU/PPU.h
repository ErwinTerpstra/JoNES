#ifndef _PPU_H_
#define _PPU_H_

#include "environment.h"
#include "nes.h"

#include "Util/Event.h"

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

		uint16_t vramAddress;
		uint16_t temporaryAddress;
		uint8_t fineX;
		bool secondWrite;

		bool nmiState;

	public:
		Event vblankStarted;

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
		void DrawDot(uint8_t x, uint8_t y);

		void DecodeTileSlice(uint16_t baseAddress, uint8_t column, uint8_t row, uint8_t y, uint8_t* buffer);
		void DecodeTileSlice(uint16_t baseAddress, uint8_t tileIndex, uint8_t y, uint8_t* buffer);
	};
}

#endif