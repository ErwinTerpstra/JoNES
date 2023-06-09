#include "Debugger.h"

#include "Emulator.h"

#include "Device.h"

#include "CPU/CPU.h"
#include "CPU/instruction.h"

#include "DebuggerMemoryInterface.h"

using namespace libnes;

Debugger::Debugger(Emulator* emulator) : emulator(emulator),
	breakpoints(32), mainMemoryBreakpoints(32), videoMemoryBreakpoints(32),
	paused(true), emulatorTime(0.0f), previousTime(0.0f), timeScale(1.0f)
{
	//mainMemoryProxy = new DebuggerMemoryInterface(this, mainMemoryBreakpoints, emulator->device->mainMemory);
	//videoMemoryProxy = new DebuggerMemoryInterface(this, videoMemoryBreakpoints, emulator->device->videoMemory);
}

Debugger::~Debugger()
{
	SAFE_DELETE(mainMemoryProxy);
	//SAFE_DELETE(videoMemoryProxy);
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

void Debugger::Reset()
{
	emulator->Reset();

	emulatorTime = 0.0f;

	paused = true;
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
			printf("[Debugger]: Execution breakpoint hit at $%04X\n", pc);
			Pause();
			break;
		}
	}
}

void Debugger::Update(float time)
{
	if (previousTime > 0.0f && !paused)
	{
		emulatorTime += (time - previousTime) * timeScale;

		float delta = emulatorTime - emulator->Time();
		if (delta > timeScale)
			emulatorTime = emulator->Time();

		while (emulatorTime > emulator->Time() && !paused)
			Step();
	}


	previousTime = time;
}