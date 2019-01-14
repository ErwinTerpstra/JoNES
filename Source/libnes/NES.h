#ifndef _NES_H_
#define _NES_H_

namespace libnes
{
	class CPU;
	class Memory;
	class Cartridge;

	struct NES
	{
		CPU* cpu;
		Memory* mainMemory;

		Cartridge* cartridge;
	};
}

#endif