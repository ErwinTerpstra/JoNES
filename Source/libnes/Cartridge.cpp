#include "Cartridge.h"

#include "debug.h"

using namespace libnes;

Cartridge::Cartridge()
{

}

void Cartridge::Load_iNES(uint8_t* buffer, uint32_t bufferSize)
{
	memcpy(&header, buffer, sizeof(CartridgeHeader_iNES));

	assert(header.GetMapper() == 0);
	assert(!header.IsNES20());

	prgRomSize = header.GetPrgRomSizeInBytes();
	chrRomSize = header.GetChrRomSizeInBytes();

	prgRom = new uint8_t[prgRomSize];
	chrRom = new uint8_t[chrRomSize];

	uint8_t* prgRomSrc = buffer + sizeof(CartridgeHeader_iNES);
	uint8_t* chrRomSrc = prgRomSrc + prgRomSize;

	memcpy(prgRom, prgRomSrc, prgRomSize);
	memcpy(chrRom, chrRomSrc, chrRomSize);
}

uint8_t Cartridge::ReadMain(uint16_t address) const
{
	assert(address >= 0x6000);

	if (address < 0x8000)
	{
		// PRG RAM area, not supported
		assert(false);
		return 0;
	}

	return prgRom[(address - 0x8000) % prgRomSize];
}

void Cartridge::WriteMain(uint16_t address, uint8_t value)
{

}

uint8_t Cartridge::ReadVideo(uint16_t address) const
{
	if (TEST_BIT(address, 13))
	{
		// Read from cartridge VRAM
		assert(false);
		return 0;
	}
	else
		return chrRom[address % chrRomSize];
}

void Cartridge::WriteVideo(uint16_t address, uint8_t value)
{
	if (TEST_BIT(address, 13))
	{
		// Write to cartridge VRAM
		assert(false);
		return;
	}
}

bool Cartridge::GetInternalVideoRamA10(uint16_t address) const
{
	switch (header.GetMirroring())
	{
		case MIRROR_HORIZONTAL:
			return TEST_BIT(address, 11);

		case MIRROR_VERTICAL:
			return TEST_BIT(address, 10);

		default:
			assert(false);
			return 0;
	}
}

bool Cartridge::GetInternalVideoRamEnabled(uint16_t address) const
{
	return TEST_BIT(address, 13);
}