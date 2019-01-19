#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include "environment.h"
#include "util.h"

namespace libnes
{
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

		bool GetFlag(Flags flag)
		{
			return READ_MASK(p, flag);
		}

		void SetFlag(Flags flag)
		{
			p = SET_MASK(p, flag);
		}

		void ClearFlag(Flags flag)
		{
			p = UNSET_MASK(p, flag);
		}

		void SetOrClearFlag(Flags flag, bool state)
		{
			p = SET_MASK_IF(p, flag, state);
		}

		void SetZNFromResult(uint8_t result)
		{
			SetOrClearFlag(FLAG_ZERO, result == 0);
			SetOrClearFlag(FLAG_NEGATIVE, READ_BIT(result, 7));
		}
	};
}

#endif
