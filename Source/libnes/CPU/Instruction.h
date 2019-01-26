#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

namespace libnes
{
	class CPU;
	
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
	
	struct Instruction
	{
		static const uint8_t ADDRESSING_MODE_LENGTHS[LAST_ADDRESSING_MODE];

		uint8_t opcode;
		uint8_t cycleCount;
		AddressingModeIdentifier addressingMode;

		const char* assembly;

		void (CPU::*handler)(const Instruction& instruction, uint16_t operandAddress);

		uint8_t length() const
		{
			return ADDRESSING_MODE_LENGTHS[addressingMode];
		}
	};
}

#endif