#ifndef _DEVICE_H_
#define _DEVICE_H_

namespace libnes
{
	class CPU;
	class PPU;

	class Input;

	class MainMemory;
	class VideoMemory;

	class Cartridge;

	struct Device
	{
		CPU* cpu;
		PPU* ppu;

		Input* input;

		MainMemory* mainMemory;
		VideoMemory* videoMemory;

		Cartridge* cartridge;
	};
}

#endif