#ifndef _DEVICE_H_
#define _DEVICE_H_

namespace libnes
{
	class CPU;
	class PPU;

	class Input;

	class MemoryBus;
	class Cartridge;

	struct Device
	{
		CPU* cpu;
		PPU* ppu;

		Input* input;

		MemoryBus* mainMemory;
		MemoryBus* videoMemory;

		Cartridge* cartridge;
	};
}

#endif