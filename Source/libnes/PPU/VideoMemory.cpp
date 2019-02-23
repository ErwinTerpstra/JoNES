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

	if (address < 0x3F00)
	{
		if (device->cartridge->GetVideoRamEnabled(address))
		{
			uint16_t ramAddress = address & NES_PPU_RAM_ADDRESS_MASK;
			ramAddress = SET_BIT_IF(ramAddress, 10, device->cartridge->GetVideoRamA10(address));

			return ram[ramAddress];
		}
		else
			return device->cartridge->ReadVideo(address);

	}
	else
		return paletteRam[address & NES_PPU_PALETTE_RAM_ADDRESS_MASK];
}

void VideoMemory::Write(uint16_t address, uint8_t value)
{
	address &= 0x3FFF;

	if (address < 0x3F00)
	{
		if (device->cartridge->GetVideoRamEnabled(address))
		{
			uint16_t ramAddress = address & NES_PPU_RAM_ADDRESS_MASK;
			ramAddress = SET_BIT_IF(ramAddress, 10, device->cartridge->GetVideoRamA10(address));

			ram[ramAddress] = value;
		}
		else
			device->cartridge->WriteVideo(address, value);
	}
	else
		paletteRam[address & NES_PPU_PALETTE_RAM_ADDRESS_MASK] = value;
}