#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include "environment.h"

namespace libnes
{
	class Cartridge;

	struct Instruction;
	struct Device;

	class Emulator
	{
	public:
		Device* device;

		uint8_t* frameBuffer;

	public:
		Emulator();
		~Emulator();

		void Reset();
		void InsertCartridge(Cartridge* cartridge);

		float Time() const;

		const Instruction& ExecuteNextInstruction();
	};
}

#endif