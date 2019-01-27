#ifndef _ALU_H_
#define _ALU_H_

#include "environment.h"
#include "util.h"
#include "Registers.h"

namespace libnes
{
	namespace ALU
	{
		static void compare(Registers& registers, uint8_t a, uint8_t b)
		{
			uint8_t result = a - b;

			registers.SetOrClearFlag(FLAG_CARRY, result <= a);
			registers.SetZNFromResult(result);
		}

		static uint8_t decrement(Registers& registers, uint8_t operand)
		{
			--operand;
			registers.SetZNFromResult(operand);
			
			return operand;
		}

		static uint8_t increment(Registers& registers, uint8_t operand)
		{
			++operand;
			registers.SetZNFromResult(operand);

			return operand;
		}

		void ora(Registers& registers, uint8_t operand)
		{
			registers.a = registers.a | operand;
			registers.SetZNFromResult(registers.a);
		}

		void and$(Registers& registers, uint8_t operand)
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


			registers.SetOrClearFlag(FLAG_CARRY, READ_BIT(DETECT_CARRY(registers.a, operand, result), 7));
			registers.SetOrClearFlag(FLAG_OVERFLOW, READ_BIT((registers.a ^ result) & (operand ^ result), 7));
			registers.SetZNFromResult(result);

			registers.a = result;
		}

		void sbc(Registers& registers, uint8_t operand)
		{
			adc(registers, ~operand);
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

		uint8_t dec(Registers& registers, uint8_t operand)
		{
			return decrement(registers, operand);
		}

		void dex(Registers& registers)
		{
			registers.x = decrement(registers, registers.x);
		}

		void dey(Registers& registers)
		{
			registers.y = decrement(registers, registers.y);
		}

		uint8_t inc(Registers& registers, uint8_t operand)
		{
			return increment(registers, operand);
		}

		void inx(Registers& registers)
		{
			registers.x = increment(registers, registers.x);
		}

		void iny(Registers& registers)
		{
			registers.y = increment(registers, registers.y);
		}

		uint8_t asl(Registers& registers, uint8_t operand)
		{
			registers.SetOrClearFlag(FLAG_CARRY, READ_BIT(operand, 7));

			operand <<= 1;

			registers.SetZNFromResult(operand);

			return operand;
		}

		uint8_t lsr(Registers& registers, uint8_t operand)
		{
			registers.SetOrClearFlag(FLAG_CARRY, READ_BIT(operand, 0));

			operand >>= 1;

			registers.SetZNFromResult(operand);

			return operand;
		}

		uint8_t rol(Registers& registers, uint8_t operand)
		{
			bool bit7 = READ_BIT(operand, 7);

			operand = SET_BIT_IF(operand << 1, 0, registers.GetFlag(FLAG_CARRY));

			registers.SetOrClearFlag(FLAG_CARRY, bit7);
			registers.SetZNFromResult(operand);

			return operand;
		}

		uint8_t ror(Registers& registers, uint8_t operand)
		{
			bool bit0 = READ_BIT(operand, 0);

			operand = SET_BIT_IF(operand >> 1, 7, registers.GetFlag(FLAG_CARRY));

			registers.SetOrClearFlag(FLAG_CARRY, bit0);
			registers.SetZNFromResult(operand);

			return operand;
		}
	}
}

#endif