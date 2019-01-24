#include "stdafx.h"

#include "TestSuite.h"
#include "TestMemoryInterface.h"

#include "libnes/libnes.h"

using namespace libnes;
using namespace JoNES;

TestSuite::TestSuite(Emulator* emulator) : emulator(emulator)
{
	// Install our memory interface
	emulator->device.mainMemory->Chain<TestMemoryInterface>();
}

void TestSuite::Start()
{
	emulator->device.cpu->registers.pc = 0xC000;
}