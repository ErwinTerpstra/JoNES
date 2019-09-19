#include "VideoMemory.h"

#include "Device.h"
#include "Cartridge/Cartridge.h"

#include "nes.h"
#include "util.h"

using namespace libnes;

VideoMemory::VideoMemory(Device* device) : device(device)
{
	ram = new uint8_t[NES_PPU_RAM_SIZE];
	paletteRam = new uint8_t[NES_PPU_PALETTE_RAM_SIZE];

	memset(ram, 0, NES_PPU_RAM_SIZE);
	memset(paletteRam, 0, NES_PPU_PALETTE_RAM_SIZE);
}

VideoMemory::~VideoMemory()
{
	SAFE_DELETE_ARRAY(ram);
	SAFE_DELETE_ARRAY(paletteRam);
}

uint8_t VideoMemory::ReadU8(uint16_t address)
{
	address &= 0x3FFF;

	if (address < 0x3F00)
	{
		if (device->cartridge->GetInternalVideoRamEnabled(address))
		{
			uint16_t ramAddress = address & NES_PPU_RAM_ADDRESS_MASK;
			ramAddress = SET_BIT_IF(ramAddress, 10, device->cartridge->GetInternalVideoRamA10(address));

			return ram[ramAddress];
		}
		else
			return device->cartridge->ReadVideo(address);

	}
	else
		return paletteRam[MapPaletteAddress(address)];
}

uint8_t VideoMemory::PeekU8(uint16_t address) const
{
	address &= 0x3FFF;

	if (address < 0x3F00)
	{
		if (device->cartridge->GetInternalVideoRamEnabled(address))
		{
			uint16_t ramAddress = address & NES_PPU_RAM_ADDRESS_MASK;
			ramAddress = SET_BIT_IF(ramAddress, 10, device->cartridge->GetInternalVideoRamA10(address));

			return ram[ramAddress];
		}
		else
			return device->cartridge->PeekVideo(address);

	}
	else
		return paletteRam[MapPaletteAddress(address)];
}

void VideoMemory::WriteU8(uint16_t address, uint8_t value)
{
	address &= 0x3FFF;

	if (address < 0x3F00)
	{
		if (device->cartridge->GetInternalVideoRamEnabled(address))
		{
			uint16_t ramAddress = address & NES_PPU_RAM_ADDRESS_MASK;
			ramAddress = SET_BIT_IF(ramAddress, 10, device->cartridge->GetInternalVideoRamA10(address));

			ram[ramAddress] = value;
		}
		else
			device->cartridge->WriteVideo(address, value);
	}
	else
		paletteRam[MapPaletteAddress(address)] = value;
}

uint16_t VideoMemory::MapPaletteAddress(uint16_t address) const
{
	address &= NES_PPU_PALETTE_RAM_ADDRESS_MASK;

	if ((address & 0x03) == 0x00)
		address = UNSET_BIT(address, 4);

	return address;
}