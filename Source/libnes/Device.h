#ifndef _DEVICE_H_
#define _DEVICE_H_

namespace libnes
{
	class CPU;
	class Memory;
	class Cartridge;

	struct Device
	{
		CPU* cpu;
		Memory* mainMemory;

		Cartridge* cartridge;
	};
}

#endif