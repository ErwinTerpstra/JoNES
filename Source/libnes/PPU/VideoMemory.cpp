#include "VideoMemory.h"

#include "Device.h"
#include "Cartridge.h"

#include "nes.h"
#include "util.h"

using namespace libnes;

VideoMemory::VideoMemory(Device* device) : device(device)
{
	ram = new uint8_t[NES_PPU_RAM_SIZE];
	paletteRam = new uint8_t[NES_PPU_PALETTE_RAM_SIZE];
}

VideoMemory::~VideoMemory()
{
	SAFE_DELETE_ARRAY(ram);
	SAFE_DELETE_ARRAY(paletteRam);
}

uint8_t VideoMemory::Read(uint16_t address)
{
	address &= 0x3FFF;

	if (address < 0x2000)
		return device->cartridge->ReadVideo(address);

	if (address < 0x3F00)
		return ram[(address - 0x2000) & NES_PPU_RAM_ADDRESS_MASK];

	if (address < 0x4000)
		return paletteRam[(address - 0x3F00) & NES_PPU_PALETTE_RAM_ADDRESS_MASK];

	assert(false);
}

void VideoMemory::Write(uint16_t address, uint8_t value)
{
	address &= 0x3FFF;

	if (address < 0x2000)
	{
		device->cartridge->WriteVideo(address, value);
		return;
	}

	if (address < 0x3F00)
	{
		ram[(address - 0x2000) & NES_PPU_RAM_ADDRESS_MASK] = value;
		return;
	}

	if (address < 0x4000)
	{
		paletteRam[(address - 0x3F00) & NES_PPU_PALETTE_RAM_ADDRESS_MASK] = value;
		return;
	}

	assert(false);
}