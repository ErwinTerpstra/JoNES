#include "Debugger.h"

#include "Emulator.h"

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
}

void Debugger::Update(float time)
{
	if (previousTime > 0.0f && !paused)
	{
		emulatorTime += time - previousTime;
		emulator->Update(emulatorTime);
	}

	previousTime = time;
}