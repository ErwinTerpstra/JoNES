#ifndef _ALU_H_
#define _ALU_H_

#include "environment.h"
#include "util.h"
#include "Registers.h"

namespace libnes
{
	namespace ALU
	{
		void ora(Registers& registers, uint8_t operand)
		{
			registers.a = registers.a | operand;
			registers.SetZNFromResult(registers.a);
		}

		void anda(Registers& registers, uint8_t operand)
		{
			registers.a = registers.a & operand;
			registers.SetZNFromResult(registers.a);
		}

		void eor(Registers& registers, uint8_t operand)
		{
			registers.a = registers.a ^ operand;
			registers.SetZNFromResult(registers.a);
		}

		void adc(Registers& registers, uint8_t operand)
		{
			uint8_t carry = registers.GetFlag(FLAG_CARRY);
			uint8_t result = registers.a + operand + carry;

			registers.SetOrClearFlag(FLAG_CARRY, READ_BIT(registers.a ^ operand ^ result, 7));
			registers.SetOrClearFlag(FLAG_OVERFLOW, READ_BIT((registers.a ^ result) & (operand ^ result), 7));
			registers.SetZNFromResult(result);

			registers.a = result;
		}

		void sbc(Registers& registers, uint8_t operand)
		{
			adc(registers, ~operand);
		}

		void compare(Registers& registers, uint8_t a, uint8_t b)
		{
			uint8_t result = a - b;

			registers.SetOrClearFlag(FLAG_CARRY, result <= a);
			registers.SetZNFromResult(result);
		}

		void cmp(Registers& registers, uint8_t operand)
		{
			compare(registers, registers.a, operand);
		}

		void cpx(Registers& registers, uint8_t operand)
		{
			compare(registers, registers.x, operand);
		}

		void cpy(Registers& registers, uint8_t operand)
		{
			compare(registers, registers.y, operand);
		}
	}
}

#endif