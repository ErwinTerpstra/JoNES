#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include "environment.h"
#include "Array.h"

namespace libnes
{
	class Emulator;

	class Debugger
	{
	private:
		const uint8_t MAX_BREAKPOINTS = 64;

		Emulator* emulator;

		Array<uint16_t> breakpoints;

		bool paused;

	public:
		Debugger(Emulator* emulator);
	}
};

#endif