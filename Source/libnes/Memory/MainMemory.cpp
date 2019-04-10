#include "MainMemory.h"

#include "nes.h"

#include "Device.h"
#include "Cartridge.h"

#include "CPU/CPU.h"
#include "PPU/PPU.h"
#include "Input/Input.h"

using namespace libnes;

MainMemory::MainMemory(Device* device) : device(device)
{
	ram = new uint8_t[NES_CPU_RAM_SIZE];
	memset(ram, 0, NES_CPU_RAM_SIZE);
}

MainMemory::~MainMemory()
{
	if (ram)
		delete[] ram;

	ram = NULL;
}

uint8_t MainMemory::Read(uint16_t address)
{
	if (address < 0x2000)
		return ram[address % NES_CPU_RAM_SIZE];
	
	if (address < 0x4000)
		return device->ppu->ReadRegister(address);

	if (address < 0x4020)
	{
		if (address == NES_CPU_REG_JOY0 || address == NES_CPU_REG_JOY1)
			return device->input->Read(address);

		return 0; // TODO: APU registers
	}

	if (device->cartridge != NULL)
		return device->cartridge->ReadMain(address);
	else
		return 0;
}

uint8_t MainMemory::Peek(uint16_t address) const
{
	if (address < 0x2000)
		return ram[address % NES_CPU_RAM_SIZE];

	if (address < 0x4000)
		return device->ppu->PeekRegister(address);

	if (address < 0x4020)
	{
		if (address == NES_CPU_REG_JOY0 || address == NES_CPU_REG_JOY1)
			return device->input->Peek(address);

		return 0; // TODO: APU registers
	}

	if (device->cartridge != NULL)
		return device->cartridge->PeekMain(address);
	else
		return 0;
}

void MainMemory::Write(uint16_t address, uint8_t value)
{
	if (address < 0x2000)
	{
		ram[address % 0x800] = value;
		return;
	}
	
	if (address < 0x4000)
	{
		device->ppu->WriteRegister(address, value);
		return;
	}

	if (address < 0x4020)
	{
		if (address == NES_CPU_REG_OAMDMA)
		{
			device->ppu->PerformOAMDMA(value);
			device->cpu->WaitForOAMDMA();
			return;
		}

		if (address == NES_CPU_REG_JOY0 || address == NES_CPU_REG_JOY1)
		{
			device->input->Write(address, value);
			return;
		}

		return; // TODO: APU registers
	}

	if (device->cartridge != NULL)
	{
		device->cartridge->WriteMain(address, value);
		return;
	}
}