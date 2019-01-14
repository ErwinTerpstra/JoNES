#include "Cartridge.h"
#include "CartridgeHeader.h"

#include "debug.h"

using namespace libnes;

Cartridge::Cartridge()
{

}

void Cartridge::Load_iNES(uint8_t* buffer, uint32_t bufferSize)
{
	CartridgeHeader_iNES* header = reinterpret_cast<CartridgeHeader_iNES*>(buffer);

	assert(header->GetMapper() == 0);
	assert(!header->IsNES20());

	prgRomSize = header->GetPrgRomSizeInBytes();
	chrRomSize = header->GetChrRomSizeInBytes();

	prgRom = new uint8_t[prgRomSize];
	chrRom = new uint8_t[chrRomSize];

	uint8_t* prgRomSrc = buffer + sizeof(header);
	uint8_t* chrRomSrc = prgRomSrc + prgRomSize;

	memcpy(prgRom, prgRomSrc, prgRomSize);
	memcpy(chrRom, chrRomSrc, chrRomSize);
}

uint8_t Cartridge::Read(uint16_t address) const
{
	assert(address >= 0x6000);

	if (address < 0x6000)
		return 0; // Not connected

	if (address < 0x8000)
	{
		// PRG RAM area, not supported
		assert(false);
		return 0;
	}

	return prgRom[address % prgRomSize];
}

void Cartridge::Write(uint16_t address, uint8_t value)
{
	assert(false);
}