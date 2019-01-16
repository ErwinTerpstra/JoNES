#ifndef _CPU_H_
#define _CPU_H_

#include "environment.h"
#include "nes.h"

namespace libnes
{
	struct Device;

	class CPU
	{
	public:
		enum Flags
		{
			FLAG_CARRY = 0,
			FLAG_ZERO = 1,
			FLAG_INTERRUPT_DISABLE = 2,
			FLAG_DECIMAL_MODE = 3,
			FLAG_BREAK = 4,
			FLAG_OVERFLOW = 6,
			FLAG_NEGATIVE = 7,
		};

		struct Registers
		{
			uint8_t a;
			uint8_t p;
			uint8_t x;
			uint8_t y;
			uint8_t s;

			uint16_t pc;
		};

		struct Instruction
		{
			uint8_t opcode;
			const char* disassemblyFormat;

			void (CPU::*handler)(uint8_t opcode, const uint8_t* operands);
		};

	private:
		static const Instruction INSTRUCTION_MAP[256];

		Device* device;

		uint64_t cycles;

	public:
		Registers registers;

	public:
		CPU(Device* device);

		void Reset();

		const Instruction& ExecuteNextInstruction();

		float Time() const
		{
			return cycles / (float) NES_NTSC_CPU_CLOCK_FREQUENCY;
		}
	};
}

#endif