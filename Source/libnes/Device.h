#ifndef _DEVICE_H_
#define _DEVICE_H_

namespace libnes
{
	class CPU;
	class MemoryBus;
	class Cartridge;

	struct Device
	{
		CPU* cpu;
		MemoryBus* mainMemory;

		Cartridge* cartridge;
	};
}

#endif