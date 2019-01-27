#include "Debugger.h"

#include "Emulator.h"

#include "CPU/cpu.h"

using namespace libnes;

Debugger::Debugger(Emulator* emulator) : emulator(emulator), breakpoints(32), paused(true), emulatorTime(0.0f), previousTime(0.0f)
{

}

void Debugger::Pause()
{
	paused = true;
}

void Debugger::Resume()
{
	paused = false;
}

void Debugger::Step()
{
	emulator->ExecuteNextInstruction();

	uint16_t pc = emulator->device->cpu->registers.pc;

	for (uint32_t breakpointIdx = 0; breakpointIdx < breakpoints.Length(); ++breakpointIdx)
	{
		if (breakpoints[breakpointIdx] == pc)
			paused = true;
	}
}

void Debugger::Update(float time)
{
	if (previousTime > 0.0f && !paused)
	{
		emulatorTime += time - previousTime;

		while (emulatorTime > emulator->Time())
			Step();
	}

	previousTime = time;
}