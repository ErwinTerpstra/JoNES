#ifndef _CPU_H_
#define _CPU_H_

#include "environment.h"
#include "nes.h"

#include "Registers.h"

namespace libnes
{
	struct Device;

	enum AddressingModeIdentifier
	{
		ADDR_IMPL,		// Implied
		ADDR_IMM,		// Immediate
		ADDR_REL,		// Relative
		ADDR_ZP,		// Zero page
		ADDR_ZPX,		// Zero page, X indexed
		ADDR_ZPY,		// Zero page, Y indexed
		ADDR_IND,		// Indirect
		ADDR_IZPX,		// Indirect from zero page, X pre-indexed
		ADDR_IZPY,		// Indirect from zero page, Y post-indexed
		ADDR_ABS,		// Absolute
		ADDR_ABSX,		// Absolute, X indexed
		ADDR_ABSY,		// Absolute, Y indexed
		ADDR_INVALID,	// For invalid opcodes

		LAST_ADDRESSING_MODE
	};

	struct AddressingMode
	{
		uint8_t instructionLength;
	};

	class CPU
	{
	public:
		struct Instruction
		{
			uint8_t opcode;
			uint8_t cycleCount;
			AddressingModeIdentifier addressingMode;

			const char* assembly;

			void (CPU::*handler)(const Instruction& instruction, uint16_t pc);
		};

	private:
		static const Instruction INSTRUCTION_MAP[256];
		static const uint8_t ADDRESSING_MODE_LENGTHS[LAST_ADDRESSING_MODE];

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

	private:
		uint16_t ResolveAddress(AddressingModeIdentifier mode, uint16_t pc) const;
		uint8_t ReadAddressed(AddressingModeIdentifier mode, uint16_t pc) const;
		void WriteAddressed(AddressingModeIdentifier mode, uint16_t pc, uint8_t value);

		void ora(const Instruction& instruction, uint16_t pc);
		void anda(const Instruction& instruction, uint16_t pc);
		void eor(const Instruction& instruction, uint16_t pc);
		void adc(const Instruction& instruction, uint16_t pc);
		void sbc(const Instruction& instruction, uint16_t pc);
		void cmp(const Instruction& instruction, uint16_t pc);
		void cpx(const Instruction& instruction, uint16_t pc);
		void cpy(const Instruction& instruction, uint16_t pc);
		void dec(const Instruction& instruction, uint16_t pc);
		void dex(const Instruction& instruction, uint16_t pc);
		void dey(const Instruction& instruction, uint16_t pc);
		void inc(const Instruction& instruction, uint16_t pc);
		void inx(const Instruction& instruction, uint16_t pc);
		void iny(const Instruction& instruction, uint16_t pc);
		void asl(const Instruction& instruction, uint16_t pc);
		void rol(const Instruction& instruction, uint16_t pc);
		void lsr(const Instruction& instruction, uint16_t pc);
		void ror(const Instruction& instruction, uint16_t pc);
		void asl_a(const Instruction& instruction, uint16_t pc);
		void rol_a(const Instruction& instruction, uint16_t pc);
		void lsr_a(const Instruction& instruction, uint16_t pc);
		void ror_a(const Instruction& instruction, uint16_t pc);


	};
}

#endif