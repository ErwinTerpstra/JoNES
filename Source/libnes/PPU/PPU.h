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

		uint64_t cycles;

		uint16_t scanline;

		uint8_t status;

	public:
		PPU(Device* device);

		void Reset();

		void Tick();

		uint8_t ReadRegister(uint16_t address);
		void WriteRegister(uint16_t address, uint8_t value);

		uint64_t MasterClockCycles() const
		{
			return cycles * NES_NTSC_PPU_CLOCK_DIVIDER;
		}

	private:
		void DrawScanline();
	};
}

#endif