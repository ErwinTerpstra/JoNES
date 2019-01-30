#include "Emulator.h"

#include "nes.h"

#include "Device.h"

#include "Memory/MemoryBus.h"
#include "Memory/MainMemory.h"

#include "CPU/CPU.h"
#include "CPU/Instruction.h"

#include "PPU/PPU.h"
#include "PPU/VideoMemory.h"

#include "Cartridge.h"

using namespace libnes;

Emulator::Emulator()
{
	device = new Device();
	device->cpu = new CPU(device);
	device->ppu = new PPU(device);
	device->mainMemory = new MemoryBus(new MainMemory(device));
	device->videoMemory = new MemoryBus(new VideoMemory(device));
}

Emulator::~Emulator()
{
	if (device)
	{
		delete device->cpu;
		delete device->ppu;

		delete device->mainMemory;
		delete device->videoMemory;

		delete device;

		device = NULL;
	}
}

void Emulator::Reset()
{
	device->cpu->Reset();
	device->ppu->Reset();
}

void Emulator::InsertCartridge(Cartridge* cartridge)
{
	device->cartridge = cartridge;
	Reset();
}

float Emulator::Time() const
{
	return device->cpu->MasterClockCycles() / (float) NES_NTSC_MASTER_CLOCK_SPEED;
}

const Instruction& Emulator::ExecuteNextInstruction()
{
	const Instruction& instruction = device->cpu->ExecuteNextInstruction();

	// Sync the PPU
	while (device->ppu->MasterClockCycles() < device->cpu->MasterClockCycles())
		device->ppu->Tick();

	return instruction;
}