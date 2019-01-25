#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include "environment.h"

#include "Util/Array.h"

namespace libnes
{
	class Emulator;

	class Debugger
	{
	private:
		const uint8_t MAX_BREAKPOINTS = 64;

		Array<uint16_t> breakpoints;

		bool paused;

		float previousTime;
		float emulatorTime;

	public:
		Emulator * emulator;

	public:
		Debugger(Emulator* emulator);
		
		void Pause();
		void Resume();
		void Step();

		void Update(float time);

		bool IsPaused() const { return paused; }
	};
}

#endif