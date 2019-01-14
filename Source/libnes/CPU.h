#ifndef _CPU_H_
#define _CPU_H_

#include "environment.h"

namespace libnes
{
	struct NES;

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

			uint16_t pc;
			uint16_t sp;
		};

		struct Instruction
		{
			uint8_t opcode;
			uint8_t length;
			uint8_t cycles;

			void (CPU::*handler)(uint8_t opcode, const uint8_t* operands);

			char* disassemblyFormat;
		};

	private:
		static const Instruction INSTRUCTION_MAP[256];

		NES* nes;

	public:
		Registers registers;

	public:
		CPU(NES* nes);

		const Instruction& ExecuteNextInstruction();
	};
}

#endif