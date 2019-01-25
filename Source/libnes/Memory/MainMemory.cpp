#include "MainMemory.h"

#include "Device.h"
#include "Cartridge.h"
#include "nes.h"

using namespace libnes;

MainMemory::MainMemory(Device* device) : device(device)
{
	ram = new uint8_t[NES_INTERNAL_RAM_SIZE];
	memset(ram, 0, NES_INTERNAL_RAM_SIZE);
}

MainMemory::~MainMemory()
{
	if (ram)
		delete[] ram;

	ram = NULL;
}

uint8_t MainMemory::Read(uint16_t address) const
{
	if (address < 0x2000)
		return ram[address % NES_INTERNAL_RAM_SIZE];
	
	if (address < 0x400)
		return 0; // TODO: PPU registers

	if (address < 0x4020)
		return 0; // TODO: APU and IO registers

	if (device->cartridge != NULL)
		return device->cartridge->Read(address);
	else
		return 0;
}

void MainMemory::Write(uint16_t address, uint8_t value)
{
	if (address < 0x2000)
		ram[address % 0x800] = value;

	if (address < 0x400)
		return; // TODO: PPU registers

	if (address < 0x4020)
		return; // TODO: APU and IO registers

	if (device->cartridge != NULL)
		return device->cartridge->Write(address, value);
}