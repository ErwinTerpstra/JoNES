#include "Debugger.h"

#include "Emulator.h"

#include "Device.h"

#include "CPU/CPU.h"
#include "CPU/instruction.h"

using namespace libnes;

Debugger::Debugger(Emulator* emulator) : emulator(emulator), breakpoints(32), paused(true), emulatorTime(0.0f), previousTime(0.0f)
{

}

void Debugger::Pause()
{
	emulatorTime = emulator->Time();
	paused = true;
}

void Debugger::Resume()
{
	paused = false;
}

void Debugger::Step()
{
	const Instruction& instruction = emulator->ExecuteNextInstruction();

	if (instruction.handler == NULL)
		Pause();

	uint16_t pc = emulator->device->cpu->registers.pc;

	for (uint32_t breakpointIdx = 0; breakpointIdx < breakpoints.Length(); ++breakpointIdx)
	{
		if (breakpoints[breakpointIdx] == pc)
		{
			Pause();
			break;
		}
	}
}

void Debugger::Update(float time)
{
	if (previousTime > 0.0f && !paused)
	{
		emulatorTime += time - previousTime;

		while (emulatorTime > emulator->Time())
		{
			Step();

			if (paused)
				break;
		}
	}

	previousTime = time;
}