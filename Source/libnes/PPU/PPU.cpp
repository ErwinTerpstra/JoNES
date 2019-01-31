#include "PPU.h"

#include "Device.h"

#include "util.h"

using namespace libnes;

PPU::PPU(Device* device) : device(device)
{

}

void PPU::Reset()
{
	cycles = 0;
	scanline = 0;

	status = 0;
}

void PPU::Tick()
{
	++cycles;

	// Update scanline
	uint32_t localCycle = cycles % NES_PPU_CYCLES_PER_SCANLINE;

	if (localCycle == 0)
	{
		DrawScanline();

		scanline = (scanline + 1) % (NES_PPU_LAST_SCANLINE + 1);
	}


	// Handle VBlank
	if (scanline == NES_PPU_VBLANK_FIRST_SCANLINE && localCycle == 1)
	{
		// Set vblank bit
		status = SET_BIT(status, NES_PPU_STATUS_BIT_VBLANK);
	}

	if (scanline == NES_PPU_PRE_RENDER_SCANLINE && localCycle == 1)
	{
		// Reset status flags
		status = 0;
	}


}

uint8_t PPU::ReadRegister(uint16_t address)
{
	switch (address & 0x07)
	{
		case 0x02:
		{
			uint8_t value = status;
			status = UNSET_BIT(status, NES_PPU_STATUS_BIT_VBLANK);
			
			return value;
		}

		default:
		case 0x00:
		case 0x01:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			return 0;
	}
}

void PPU::WriteRegister(uint16_t address, uint8_t value)
{
	switch (address & 0x07)
	{
	
	}
}

void PPU::DrawScanline()
{

}