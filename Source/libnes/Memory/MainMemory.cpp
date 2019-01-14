#include "MainMemory.h"

#include "NES.h"
#include "Cartridge.h"

using namespace libnes;

MainMemory::MainMemory(NES* nes) : nes(nes)
{
	ram = new uint8_t[0x0800];
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
		return ram[address % 0x800];
	
	if (address < 0x400)
		return 0; // TODO: PPU registers

	if (address < 0x4020)
		return 0; // TODO: APU and IO registers

	if (nes->cartridge != NULL)
		return nes->cartridge->Read(address);
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

	if (nes->cartridge != NULL)
		return nes->cartridge->Write(address, value);
}