#include "Cartridge.h"

#include "Mapper.h"
#include "NROM.h"
#include "SxROM.h"

#include "debug.h"

using namespace libnes;

Cartridge::Cartridge() : mapper(NULL)
{

}

void Cartridge::Load_iNES(uint8_t* buffer, uint32_t bufferSize)
{
	memcpy(&header, buffer, sizeof(CartridgeHeader_iNES));
	assert(!header.IsNES20());

	// Delete previous mapper
	SAFE_DELETE(mapper);

	uint8_t mapperType = header.GetMapper();
	switch (mapperType)
	{
		case 0:
			mapper = new NROM(header, buffer);
			break;

		case 1:
			mapper = new SxROM(header, buffer);
			break;

		default:
			assert(false);
			break;
	}
}

uint8_t Cartridge::ReadMain(uint16_t address)
{
	return mapper->ReadMain(address);
}

uint8_t Cartridge::PeekMain(uint16_t address) const
{
	return mapper->PeekMain(address);
}

void Cartridge::WriteMain(uint16_t address, uint8_t value)
{
	mapper->WriteMain(address, value);
}

uint8_t Cartridge::ReadVideo(uint16_t address)
{
	return mapper->ReadVideo(address);
}

uint8_t Cartridge::PeekVideo(uint16_t address) const
{
	return mapper->PeekVideo(address);
}

void Cartridge::WriteVideo(uint16_t address, uint8_t value)
{
	mapper->WriteVideo(address, value);
}

bool Cartridge::GetInternalVideoRamA10(uint16_t address) const
{
	return mapper->GetInternalVideoRamA10(address);
}

bool Cartridge::GetInternalVideoRamEnabled(uint16_t address) const
{
	return mapper->GetInternalVideoRamEnabled(address);
}