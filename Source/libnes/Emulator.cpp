#include "Emulator.h"

#include "Device.h"

#include "CPU/CPU.h"
#include "Memory/MemoryBus.h"
#include "Memory/MainMemory.h"

#include "Cartridge.h"


using namespace libnes;

Emulator::Emulator()
{
	device = new Device();
	device->cpu = new CPU(device);
	device->mainMemory = new MemoryBus(new MainMemory(device));
}

Emulator::~Emulator()
{
	if (device)
	{
		delete device;
		device = NULL;
	}
}

void Emulator::Reset()
{
	device->cpu->Reset();
}

void Emulator::InsertCartridge(Cartridge* cartridge)
{
	device->cartridge = cartridge;
	Reset();
}

void Emulator::Update(float time)
{
	while (device->cpu->Time() < time)
		device->cpu->ExecuteNextInstruction();
}

void Emulator::ExecuteNextInstruction()
{
	device->cpu->ExecuteNextInstruction();
}