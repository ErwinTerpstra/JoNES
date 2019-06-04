#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include "environment.h"

#include "Util/Vector.h"

namespace libnes
{
	class Emulator;
	class DebuggerMemoryInterface;

	struct MemoryBreakpoint
	{
		uint16_t address;
		bool read, write;
	};

	class Debugger
	{
	private:
		DebuggerMemoryInterface* mainMemoryProxy;
		DebuggerMemoryInterface* videoMemoryProxy;

		bool paused;

		float previousTime;
		float emulatorTime;

		float timeScale;

	public:
		Emulator* emulator;

		Vector<uint16_t> breakpoints;
		Vector<MemoryBreakpoint> mainMemoryBreakpoints;
		Vector<MemoryBreakpoint> videoMemoryBreakpoints;

	public:
		Debugger(Emulator* emulator);
		~Debugger();
		
		void Pause();
		void Resume();
		void Step();
		void Reset();

		void Update(float time);

		bool IsPaused() const { return paused; }
		
		float GetTimeScale() const { return timeScale; }
		void SetTimeScale(float timeScale) { this->timeScale = timeScale; }
	};
}

#endif