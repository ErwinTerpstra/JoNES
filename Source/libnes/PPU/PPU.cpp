#include "PPU.h"

#include "Device.h"
#include "CPU/CPU.h"
#include "Memory/MemoryBus.h"

#include "util.h"

using namespace libnes;

const uint8_t NES_2C02_PALETTE_COLORS[] =
{
	84, 84, 84, 0, 30, 116, 8, 16, 144, 48, 0, 136, 68, 0, 100, 92, 0, 48, 84, 4, 0, 60, 24, 0, 32, 42, 0, 8, 58, 0, 0, 64, 0, 0, 60, 0, 0, 50, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	152, 150, 152, 8, 76, 196, 48, 50, 236, 92, 30, 228, 136, 20, 176, 160, 20, 100, 152, 34, 32, 120, 60, 0, 84, 90, 0, 40, 114, 0, 8, 124, 0, 0, 118, 40, 0, 102, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	236, 238, 236, 76, 154, 236, 120, 124, 236, 176, 98, 236, 228, 84, 236, 236, 88, 180, 236, 106, 100, 212, 136, 32, 160, 170, 0, 116, 196, 0, 76, 208, 32, 56, 204, 108, 56, 180, 204, 60, 60, 60, 0, 0, 0, 0, 0, 0,
	236, 238, 236, 168, 204, 236, 188, 188, 236, 212, 178, 236, 236, 174, 236, 236, 174, 212, 236, 180, 176, 228, 196, 144, 204, 210, 120, 180, 222, 120, 168, 226, 144, 152, 226, 180, 160, 214, 228, 160, 162, 160, 0, 0, 0, 0, 0, 0,
};

PPU::PPU(Device* device, uint8_t* frameBuffer) : device(device), frameBuffer(frameBuffer)
{
	oam = new uint8_t[NES_PPU_OAM_SIZE];
}

PPU::~PPU()
{
	SAFE_DELETE_ARRAY(oam);
}


void PPU::Reset()
{
	cycles = 0;
	scanline = 0;

	statusRegister = 0;
	controlBits = 0;
	maskBits = 0;
	oamAddress = 0;

	vramAddress = 0;
	temporaryAddress = 0;
	fineX = 0;
	secondWrite = false;

	memset(&registers, 0, sizeof(PPU_Registers));

	nmiState = false;
}

void PPU::Tick()
{
	// Check which dot in the current scanline to update
	uint32_t dot = cycles % NES_PPU_CYCLES_PER_SCANLINE;

	// Visible scanlines
	if (scanline < NES_PPU_VBLANK_FIRST_SCANLINE)
	{
		if (dot >= 1 && dot <= 256)
			DrawDot((uint8_t) (dot - 1), (uint8_t) scanline);
	}

	// Increment coarse X each 8th dot
	if (dot != 0 && (dot & 0x7) == 0)
		IncrementCourseX();

	if (dot == 256)
		IncrementY(); // Increment fine Y after the last visible dot
	else if (dot == 257)
		ResetHorizontal(); // Reset horizontal part of VRAM address
		
	// Handle VBlank
	if (scanline == NES_PPU_VBLANK_FIRST_SCANLINE && dot == 1)
	{
		// Set vblank bit
		statusRegister = SET_BIT(statusRegister, NES_PPU_STATUS_BIT_VBLANK);
		vblankStarted.Fire();
	}

	if (scanline == NES_PPU_PRE_RENDER_SCANLINE)
	{
		// Reset status flags
		if (dot == 1)
			statusRegister = 0;

		// Reset vertical part of VRAM address
		if (dot >= 280 && dot <= 304)
			ResetVertical();
	}

	// Handle NMI
	bool nmiActive = READ_BIT(controlBits, NES_PPU_CONTROL_BIT_NMI_ENABLE) && READ_BIT(statusRegister, NES_PPU_STATUS_BIT_VBLANK);
	if (nmiActive && !nmiState)
		device->cpu->TriggerNMI();

	nmiState = nmiActive;

	// Increment scanline
	if (dot == (NES_PPU_CYCLES_PER_SCANLINE - 1))
		scanline = (scanline + 1) % (NES_PPU_LAST_SCANLINE + 1);

	// Increment cycle count
	++cycles;
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
			secondWrite = false;
			
			return value;
		}

		case 0x04:
			return oam[oamAddress];

		case 0x07:
		{
			uint8_t value = device->videoMemory->ReadU8(vramAddress);
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
			temporaryAddress = (temporaryAddress & 0xC00) | (((uint16_t) (value & 0x3)) << 10);
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
			if (secondWrite)
			{
				temporaryAddress &= 0x0C1F;

				// Fine Y-scroll
				temporaryAddress |= ((uint16_t) (value & 0x7)) << 12;

				// Coarse Y-scroll
				temporaryAddress |= ((uint16_t) (value >> 3)) << 5;
			}
			else
			{
				// Fine X-scroll
				fineX = value & 0x7;

				// Coarse X-scroll
				temporaryAddress = (temporaryAddress & ~0x1F) | (value >> 3);
			}

			secondWrite = !secondWrite;
			break;

		case 0x06:
			if (secondWrite)
			{
				// Address LSB
				temporaryAddress = (temporaryAddress & 0xFF00) | value;
				vramAddress = temporaryAddress;
			}
			else
			{
				// Address MSB
				temporaryAddress = (temporaryAddress & 0x00FF) | ((value & 0x3F) << 8);
			}

			secondWrite = !secondWrite;
			break;

		case 0x07:
			device->videoMemory->WriteU8(vramAddress, value);
			IncrementAddress();
			break;
	}
}

void PPU::PerformOAMDMA(uint8_t addressMSB)
{
	uint16_t address = addressMSB << 8;

	for (uint16_t offset = 0; offset <= 0xFF; ++offset)
		oam[oamAddress + offset] = device->mainMemory->ReadU8(address + offset);
}

void PPU::IncrementAddress()
{
	// Read the increment mode bit to check if we should increment by 32 (one row) or 1 (one column)
	if (READ_BIT(controlBits, NES_PPU_CONTROL_BIT_INCREMENT_MODE))
		vramAddress += 32;
	else
		vramAddress += 1;

	vramAddress &= 0x3FFF;
}

void PPU::IncrementCourseX()
{
	// Check if course X is 32 (0x1F)
	if (READ_MASK(vramAddress, 0x1F))
	{
		// Reset course X
		vramAddress = UNSET_MASK(vramAddress, 0x1F);
		
		// Switch horizontal nametable
		vramAddress ^= 0x400;
	}
	else
		++vramAddress;
}

void PPU::IncrementY()
{
	// CHeck if fine Y is 7
	if (READ_MASK(vramAddress, 0x7000))
	{
		// Reset fine Y
		vramAddress = UNSET_MASK(vramAddress, 0x7000);
		
		// Read coarse Y
		uint16_t coarseY = (vramAddress & 0x03E0) >> 5;

		if (coarseY == 29)
		{
			// Reset coarse Y
			vramAddress = UNSET_MASK(vramAddress, 0x03E0);

			// Switch vertical nametable
			vramAddress ^= 0x0800;
		}
		else if (coarseY == 31)
		{
			// This case can happen when a program writes a value >= 240 to $2005

			// Reset coarse Y
			vramAddress = UNSET_MASK(vramAddress, 0x03E0);
		}
		else
		{
			// Increment coarse Y
			vramAddress += 0x10;
		}
	}
	else
		vramAddress += 0x1000;
}

void PPU::ResetHorizontal()
{
	vramAddress = COPY_MASK(vramAddress, temporaryAddress, 0x41F);
}

void PPU::ResetVertical()
{
	vramAddress = COPY_MASK(vramAddress, temporaryAddress, 0x7BE0);
}

void PPU::FetchData()
{
	// Check which pattern table to use for the background
	uint16_t basePatternTableAddress = READ_BIT(controlBits, NES_PPU_CONTROL_BIT_BACKGROUND_ADDRESS) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;

	// Determine nametable and attribute addresses
	uint16_t v = vramAddress;
	uint16_t nametableAddress = 0x2000 | (v & 0x0FFF);
	uint16_t attributeAddress = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);

	// Determine location of tile in pattern table
	uint8_t patternTableIndex = device->videoMemory->ReadU8(nametableAddress);

	// Determine the address of the tile
	uint8_t fineY = vramAddress >> 12;
	uint16_t patternTableAddress = basePatternTableAddress | (patternTableIndex << 4) | (fineY & 0x07);

	// Read the data for the upper and lower planes
	uint8_t lowerPlane = device->videoMemory->ReadU8(patternTableAddress);
	uint8_t upperPlane = device->videoMemory->ReadU8(patternTableAddress | 0x08);

	// Place the data in the upper 8 bits of the shift registers
	registers.tileDataHigh = (registers.tileDataHigh & 0xFF) | (upperPlane << 8);
	registers.tileDataLow = (registers.tileDataLow & 0xFF) | (lowerPlane << 8);
}

void PPU::DrawDot(uint8_t x, uint8_t y)
{
	// Check which pattern table to use for the background
	uint16_t basePatternTableAddress = READ_BIT(controlBits, NES_PPU_CONTROL_BIT_BACKGROUND_ADDRESS) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;

	// Determine nametable and attribute addresses
	uint16_t v = vramAddress;
	uint16_t nametableAddress = 0x2000 | (v & 0x0FFF);
	uint16_t attributeAddress = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);

	// Determine location of tile in pattern table
	uint8_t patternTableIndex = device->videoMemory->ReadU8(nametableAddress);

	// Decode the slice containing the current pixel
	uint8_t fineY = vramAddress >> 12;
	uint8_t paletteIndex = DecodeTilePixel(basePatternTableAddress, patternTableIndex, fineX, fineY);

	// Set bit 4 when selecting palette index for background tiles 
	paletteIndex = SET_BIT(paletteIndex, 4);
		
	// TODO: attribute table bits
		
	// Read palette color
	uint16_t paletteAddress = NES_PPU_PALETTE_RAM | paletteIndex;
	uint8_t color = device->videoMemory->ReadU8(paletteAddress);

	// Decode color and write to framebuffer
	DecodeColor(color, frameBuffer + (y * NES_FRAME_WIDTH + x) * 3);
}

void PPU::DecodeColor(uint8_t color, uint8_t* buffer) const
{
	buffer[0] = NES_2C02_PALETTE_COLORS[color * 3 + 0];
	buffer[1] = NES_2C02_PALETTE_COLORS[color * 3 + 1];
	buffer[2] = NES_2C02_PALETTE_COLORS[color * 3 + 2];
}

void PPU::DecodePatternTable(uint16_t address, uint8_t* buffer)
{
	for (uint8_t row = 0; row < NES_PPU_PATTERN_TABLE_TILES_PER_ROW; ++row)
	{
		for (uint8_t y = 0; y < NES_PPU_TILE_SIZE; ++y)
		{
			for (uint8_t column = 0; column < NES_PPU_PATTERN_TABLE_TILES_PER_COLUMN; ++column)
			{
				DecodeTileSlice(address, column, row, y, buffer);
				buffer += NES_PPU_TILE_SIZE;
			}
		}
	}
}

void PPU::DecodeTileSlice(uint16_t baseAddress, uint8_t column, uint8_t row, uint8_t y, uint8_t* buffer)
{
	return DecodeTileSlice(baseAddress, (row << 3) + column, y, buffer);
}

void PPU::DecodeTileSlice(uint16_t baseAddress, uint8_t tileIndex, uint8_t y, uint8_t* buffer)
{
	uint16_t address = baseAddress | (tileIndex << 4) | (y & 0x07);
	uint8_t lowerPlane = device->videoMemory->ReadU8(address);
	uint8_t upperPlane = device->videoMemory->ReadU8(address | 0x08);

	for (uint8_t x = 0; x < NES_PPU_TILE_SIZE; ++x)
		buffer[x] = (READ_BIT(upperPlane, 7 - x) << 1) | READ_BIT(lowerPlane, 7 - x);
}

uint8_t PPU::DecodeTilePixel(uint16_t baseAddress, uint8_t tileIndex, uint8_t x, uint8_t y)
{
	uint16_t address = baseAddress | (tileIndex << 4) | (y & 0x07);
	uint8_t lowerPlane = device->videoMemory->ReadU8(address);
	uint8_t upperPlane = device->videoMemory->ReadU8(address | 0x08);

	return (READ_BIT(upperPlane, 7 - x) << 1) | READ_BIT(lowerPlane, 7 - x);
}