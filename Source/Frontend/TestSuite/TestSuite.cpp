#include "stdafx.h"

#include "TestSuite.h"
#include "TestMemoryInterface.h"

#include "File.h"

#include "libnes/libnes.h"

using namespace libnes;
using namespace JoNES;

TestSuite::TestSuite(Emulator* emulator, const char* logFileName) : emulator(emulator), logFileName(logFileName)
{
	// Install our memory interface
	emulator->device->mainMemory->Chain<TestMemoryInterface>();
}

void TestSuite::Start()
{
	emulator->device->cpu->registers.pc = 0xC000;
}

void TestSuite::ExecuteNextInstruction()
{
	WriteCurrentStateToLog();
	emulator->ExecuteNextInstruction();
}

void TestSuite::WriteCurrentStateToLog()
{
	CPU* cpu = emulator->device->cpu;

	const CPU::Instruction& instruction = cpu->DecodeInstruction(cpu->registers.pc);


}