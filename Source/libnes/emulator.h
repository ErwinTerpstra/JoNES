#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include "Device.h"

namespace libnes
{
	class Cartridge;

	class Emulator
	{
	public:
		Device* device;

	public:
		Emulator();
		~Emulator();

		void Reset();

		void InsertCartridge(Cartridge* cartridge);

		void Update(float time);
		void ExecuteNextInstruction();
	};
}

#endif