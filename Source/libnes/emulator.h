#ifndef _EMULATOR_H_
#define _EMULATOR_H_

namespace libnes
{
	class Cartridge;

	struct Instruction;
	struct Device;

	class Emulator
	{
	public:
		Device* device;

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