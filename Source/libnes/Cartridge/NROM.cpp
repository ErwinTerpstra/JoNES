#include "NROM.h"

#include "CartridgeHeader.h"

using namespace libnes;

NROM::NROM(const CartridgeHeader_iNES& header, const uint8_t* buffer) : Mapper(header)
{
	prgRomSize = header.GetPrgRomSizeInBytes();
	chrRomSize = header.GetChrRomSizeInBytes();

	prgRom = new uint8_t[prgRomSize];
	chrRom = new uint8_t[chrRomSize];

	const uint8_t* prgRomSrc = buffer + sizeof(CartridgeHeader_iNES);
	const uint8_t* chrRomSrc = prgRomSrc + prgRomSize;

	memcpy(prgRom, prgRomSrc, prgRomSize);
	memcpy(chrRom, chrRomSrc, chrRomSize);
}

NROM::~NROM()
{
	SAFE_DELETE_ARRAY(prgRom);
	SAFE_DELETE_ARRAY(chrRom);
}

uint8_t NROM::PeekMain(uint16_t address) const
{
	if (address < 0x8000)
	{
		// PRG RAM area, not supported
		return 0;
	}

	return prgRom[(address - 0x8000) % prgRomSize];
}


void NROM::WriteMain(uint16_t address, uint8_t value)
{

}

uint8_t NROM::PeekVideo(uint16_t address) const
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

void NROM::WriteVideo(uint16_t address, uint8_t value)
{
	if (TEST_BIT(address, 13))
	{
		// Write to cartridge VRAM
		assert(false);
		return;
	}
}