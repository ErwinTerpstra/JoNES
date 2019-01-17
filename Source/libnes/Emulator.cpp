#include "Emulator.h"

#include "Device.h"

#include "CPU.h"
#include "Memory/MemoryBus.h"
#include "Memory/MainMemory.h"

#include "Cartridge.h"


using namespace libnes;

Emulator::Emulator()
{
	device.cpu = new CPU(&device);
	device.mainMemory = new MemoryBus(*(new MainMemory(&device)));
}

Emulator::~Emulator()
{

}

void Emulator::Reset()
{
	device.cpu->Reset();
}

void Emulator::InsertCartridge(Cartridge* cartridge)
{
	device.cartridge = cartridge;
}

void Emulator::Update(float time)
{
	while (device.cpu->Time() < time)
		device.cpu->ExecuteNextInstruction();
}