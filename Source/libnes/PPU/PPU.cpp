#include "PPU.h"

#include "Device.h"
#include "CPU/CPU.h"
#include "Memory/MemoryBus.h"

#include "util.h"

using namespace libnes;

PPU::PPU(Device* device) : device(device)
{
	oam = new uint8_t[NES_PPU_OAM_SIZE];
}

PPU::~PPU()
{
	if (oam)
	{
		delete[] oam;
		oam = NULL;
	}
}


void PPU::Reset()
{
	cycles = 0;
	scanline = 0;

	statusRegister = 0;
	controlBits = 0;
	maskBits = 0;
	oamAddress = 0;

	scrollX = 0;
	scrollY = 0;
	addressRegister = 0;
	writeLSB = false;

	nmiState = false;
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
		statusRegister = SET_BIT(statusRegister, NES_PPU_STATUS_BIT_VBLANK);
	}

	if (scanline == NES_PPU_PRE_RENDER_SCANLINE && localCycle == 1)
	{
		// Reset status flags
		statusRegister = 0;
	}

	// Handle NMI
	bool nmiActive = READ_BIT(controlBits, NES_PPU_CONTROL_BIT_NMI_ENABLE) && READ_BIT(statusRegister, NES_PPU_STATUS_BIT_VBLANK);
	if (nmiActive && !nmiState)
		device->cpu->TriggerNMI();

	nmiState = nmiActive;
}

uint8_t PPU::ReadRegister(uint16_t address)
{
	switch (address & 0x07)
	{
		case 0x02:
		{
			uint8_t value = statusRegister;

			// Unset VBlank bit
			statusRegister = UNSET_BIT(statusRegister, NES_PPU_STATUS_BIT_VBLANK);
			writeLSB = false;
			
			return value;
		}

		case 0x04:
			return oam[oamAddress];

		case 0x07:
		{
			uint8_t value = device->videoMemory->ReadU8(addressRegister);
			IncrementAddress();

			return value;
		}

		default:
			return 0;
	}
}

void PPU::WriteRegister(uint16_t address, uint8_t value)
{
	switch (address & 0x07)
	{
		case 0x00:
			controlBits = value;
			break;

		case 0x01:
			maskBits = value;
			break;

		case 0x03:
			oamAddress = value;
			break;

		case 0x04:
			oam[oamAddress++] = value;
			break;

		case 0x05:
			if (writeLSB)
				scrollY = value;
			else
				scrollX = value;

			writeLSB = !writeLSB;
			break;

		case 0x06:
			if (writeLSB)
				address = (address & 0xFF00) | value;
			else
				address = (address & 0x00FF) | (value << 8);

			writeLSB = !writeLSB;
			break;

		case 0x07:
			device->videoMemory->WriteU8(addressRegister, value);
			IncrementAddress();
			break;
	}
}

void PPU::PerformOAMDMA(uint8_t addressMSB)
{
	uint16_t address = addressMSB << 8;

	for (uint8_t offset = 0; offset <= 0xFF; ++offset)
		oam[oamAddress + offset] = device->mainMemory->ReadU8(address + offset);
}

void PPU::IncrementAddress()
{
	if (READ_BIT(controlBits, NES_PPU_CONTROL_BIT_INCREMENT_MODE))
		addressRegister += 32;
	else
		addressRegister += 1;
}

void PPU::DrawScanline()
{

}