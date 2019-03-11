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

	memset(&registers, 0, sizeof(PPU_Registers));
	memset(&latches, 0, sizeof(PPU_Latches));

	nmiState = false;
}

void PPU::Tick()
{
	// Check which dot in the current scanline to update
	uint32_t dot = cycles % NES_PPU_CYCLES_PER_SCANLINE;

	bool drawBackground = READ_BIT(registers.maskBits, NES_PPU_MASK_BIT_BACKGROUND);
	bool drawSprites = READ_BIT(registers.maskBits, NES_PPU_MASK_BIT_SPRITES);

	if (drawBackground || drawSprites)
	{
		// Fetch data and increment vram address
		if (scanline < NES_FRAME_HEIGHT || scanline == NES_PPU_PRE_RENDER_SCANLINE)
		{
			uint8_t localDot = dot & 0x07;

			if ((dot >= 2 && dot <= 257) || (dot >= 322 && dot <= 337))
			{
				ShiftData();

				// Fetch data from memory at the last dot of each 8-dot cycle
				if (localDot == 0)
				{
					// Load new data into the data latches
					FetchData();

					// After fetching, increment course X
					IncrementCourseX();
				}

				// Load new data into shift registers at the first dot of each 8-dot cycle
				if (localDot == 1)
					LoadShiftRegisters();
			}

			// Increment fine Y after the last visible dot
			if (dot == 256)
				IncrementY();

			// Reset horizontal part of VRAM addresss
			if (dot == 257)
				ResetHorizontal();
		}

		// Draw dots on visible scanlines
		if (scanline < NES_FRAME_HEIGHT)
		{
			if (dot >= 1 && dot <= 256)
				DrawDot((uint8_t)(dot - 1), (uint8_t)scanline);
		}
	}
		
	// Handle VBlank
	if (scanline == NES_PPU_VBLANK_FIRST_SCANLINE && dot == 1)
	{
		// Set vblank bit
		registers.statusRegister = SET_BIT(registers.statusRegister, NES_PPU_STATUS_BIT_VBLANK);
		vblankStarted.Fire();
	}

	// Handle pre-render scanline
	if (scanline == NES_PPU_PRE_RENDER_SCANLINE)
	{
		// Reset status flags
		if (dot == 1)
			registers.statusRegister = 0;

		// Reset vertical part of VRAM address
		if (dot >= 280 && dot <= 304)
			ResetVertical();
	}

	// Handle NMI
	bool nmiActive = TEST_BIT(registers.controlBits, NES_PPU_CONTROL_BIT_NMI_ENABLE) && TEST_BIT(registers.statusRegister, NES_PPU_STATUS_BIT_VBLANK);
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
			uint8_t value = registers.statusRegister;

			// Unset VBlank bit
			registers.statusRegister = UNSET_BIT(registers.statusRegister, NES_PPU_STATUS_BIT_VBLANK);
			registers.secondWrite = false;
			
			return value;
		}

		case 0x04:
			return oam[registers.oamAddress];

		case 0x07:
		{
			uint8_t value = device->videoMemory->ReadU8(registers.vramAddress);
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
		{
			registers.controlBits = value;

			uint16_t mask = ((value & 0x03) << 10);
			registers.temporaryAddress = (registers.temporaryAddress & ~0x0C00) | mask;
			break;
		}

		case 0x01:
			registers.maskBits = value;
			break;

		case 0x03:
			registers.oamAddress = value;
			break;

		case 0x04:
			oam[registers.oamAddress] = value;
			++registers.oamAddress;
			break;

		case 0x05:
			if (registers.secondWrite)
			{
				registers.temporaryAddress &= 0x0C1F;

				// Fine Y-scroll
				registers.temporaryAddress |= (value & 0x07) << 12;

				// Coarse Y-scroll
				registers.temporaryAddress |= (value & 0xF8) << 2;
			}
			else
			{
				// Fine X-scroll
				registers.fineX = value & 0x07;

				// Coarse X-scroll
				registers.temporaryAddress = (registers.temporaryAddress & ~0x1F) | (value >> 3);
			}

			registers.secondWrite = !registers.secondWrite;
			break;

		case 0x06:
			if (registers.secondWrite)
			{
				// Address LSB
				registers.temporaryAddress = (registers.temporaryAddress & 0xFF00) | value;
				registers.vramAddress = registers.temporaryAddress;
			}
			else
			{
				// Address MSB
				registers.temporaryAddress = (registers.temporaryAddress & 0x00FF) | ((value & 0x3F) << 8);
			}

			registers.secondWrite = !registers.secondWrite;
			break;

		case 0x07:
			device->videoMemory->WriteU8(registers.vramAddress, value);
			IncrementAddress();
			break;
	}
}

void PPU::PerformOAMDMA(uint8_t addressMSB)
{
	uint16_t address = addressMSB << 8;

	for (uint16_t offset = 0; offset <= 0xFF; ++offset)
		oam[registers.oamAddress + offset] = device->mainMemory->ReadU8(address + offset);
}

void PPU::IncrementAddress()
{
	// Read the increment mode bit to check if we should increment by 32 (one row) or 1 (one column)
	if (TEST_BIT(registers.controlBits, NES_PPU_CONTROL_BIT_INCREMENT_MODE))
		registers.vramAddress += 32;
	else
		registers.vramAddress += 1;

	registers.vramAddress &= 0x3FFF;
}

void PPU::IncrementCourseX()
{
	// Check if course X is 31 (0x1F)
	if (TEST_MASK(registers.vramAddress, 0x1F))
	{
		// Reset course X
		registers.vramAddress = UNSET_MASK(registers.vramAddress, 0x1F);
		
		// Switch horizontal nametable
		registers.vramAddress ^= 0x400;
	}
	else
	{
		// Increment course X
		registers.vramAddress += 0x01;
	}
}

void PPU::IncrementY()
{
	// CHeck if fine Y is 7
	if (TEST_MASK(registers.vramAddress, 0x7000))
	{
		// Reset fine Y
		registers.vramAddress = UNSET_MASK(registers.vramAddress, 0x7000);
		
		// Read coarse Y
		uint16_t coarseY = (registers.vramAddress & 0x03E0) >> 5;

		if (coarseY == 29)
		{
			// Reset coarse Y
			registers.vramAddress = UNSET_MASK(registers.vramAddress, 0x03E0);

			// Switch vertical nametable
			registers.vramAddress ^= 0x0800;
		}
		else if (coarseY == 31)
		{
			// This case can happen when a program writes a value >= 240 to $2005

			// Reset coarse Y
			registers.vramAddress = UNSET_MASK(registers.vramAddress, 0x03E0);
		}
		else
		{
			// Increment coarse Y
			registers.vramAddress += 0x20;
		}
	}
	else
	{
		// Increment fine Y
		registers.vramAddress += 0x1000;
	}
}

void PPU::ResetHorizontal()
{
	registers.vramAddress = COPY_MASK(registers.vramAddress, registers.temporaryAddress, 0x041F);
}

void PPU::ResetVertical()
{
	registers.vramAddress = COPY_MASK(registers.vramAddress, registers.temporaryAddress, 0xFBE0);
}

void PPU::FetchData()
{
	// Check which pattern table to use for the background
	uint16_t basePatternTableAddress = TEST_BIT(registers.controlBits, NES_PPU_CONTROL_BIT_BACKGROUND_ADDRESS) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;

	// Determine nametable and attribute addresses
	uint16_t v = registers.vramAddress;
	uint16_t nametableAddress = 0x2000 | (v & 0x0FFF);

	// Determine location of tile in pattern table
	uint8_t patternTableIndex = device->videoMemory->ReadU8(nametableAddress);

	// Determine the address of the tile
	uint8_t fineY = (registers.vramAddress >> 12) & 0x07;
	uint16_t patternTableAddress = basePatternTableAddress | (patternTableIndex << 4) | fineY;

	// Read the data for the upper and lower planes
	latches.tileLow = device->videoMemory->ReadU8(patternTableAddress);
	latches.tileHigh = device->videoMemory->ReadU8(patternTableAddress | 0x08);

	// Read the attribute data for the 4x4 tile area this tile is part of
	uint16_t attributeAddress = 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
	uint8_t attributeData = device->videoMemory->ReadU8(attributeAddress);
	
	// Shift the attribute bits for this tile to bit 0 & 1
	attributeData >>= (READ_BIT(v, 1) << 1); // Shift 2 if coarse X bit 1 is set
	attributeData >>= (READ_BIT(v, 6) << 2); // Shift 4 if coarse Y bit 1 is set

	// Store bits 0 & 1 in the attribute data latch
	latches.attribute = attributeData & 0x03;
}

void PPU::LoadShiftRegisters()
{
	// Place the tile data in the lower 8 bits of the shift registers
	registers.tileDataLow = (registers.tileDataLow & 0xFF00) | latches.tileLow;
	registers.tileDataHigh = (registers.tileDataHigh & 0xFF00) | latches.tileHigh;
}

void PPU::ShiftData()
{
	// Shift tile data registers
	registers.tileDataLow <<= 1;
	registers.tileDataHigh <<= 1;

	// Shift attribute data registers
	registers.attributeLow <<= 1;
	registers.attributeHigh <<= 1;

	// Fill lowest bits of attribute data registers
	registers.attributeLow |= READ_BIT(latches.attribute, 0);
	registers.attributeHigh |= READ_BIT(latches.attribute, 1);
}

void PPU::DrawDot(uint8_t x, uint8_t y)
{
	uint8_t tileBit = 15 - registers.fineX;
	uint8_t attributeBit = 7 - registers.fineX;

	// Get bit 0 & 1 of the palette index from the top bit in the tile data
	uint8_t paletteIndex = 0;
	paletteIndex |= READ_BIT(registers.tileDataLow, tileBit) << 0;
	paletteIndex |= READ_BIT(registers.tileDataHigh, tileBit) << 1;

	// Get bit 2 & 3 of the palette index from the attribute data
	paletteIndex |= READ_BIT(registers.attributeLow, attributeBit) << 2;
	paletteIndex |= READ_BIT(registers.attributeHigh, attributeBit) << 3;
			
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

void PPU::DecodeNametable(uint16_t address, uint8_t* buffer)
{
	// Check which pattern table to use for the background
	uint16_t basePatternTableAddress = TEST_BIT(registers.controlBits, NES_PPU_CONTROL_BIT_BACKGROUND_ADDRESS) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;

	for (uint8_t coarseY = 0; coarseY < NES_PPU_NAMETABLE_TILES_PER_COLUMN; ++coarseY)
	{
		for (uint8_t fineY = 0; fineY < NES_PPU_TILE_SIZE; ++fineY)
		{
			for (uint8_t coarseX = 0; coarseX < NES_PPU_NAMETABLE_TILES_PER_ROW; ++coarseX)
			{
				// Determine the location of the tile in the nametable
				uint16_t nametableAddress = address | (coarseY << 5) | coarseX;
				uint16_t attributeAddress = address | 0x03C0 | ((coarseY >> 2) << 3) | (coarseX >> 2);

				// Determine location of tile in pattern table
				uint8_t patternTableIndex = device->videoMemory->ReadU8(nametableAddress);

				// Read the attribute data for this tile
				uint8_t attributeBits = device->videoMemory->ReadU8(attributeAddress);

				// Shift the attribute bits to bit 0 & 1
				attributeBits >>= (READ_BIT(coarseX, 1) << 1);		// Shift 2 if coarse X bit 1 is set
				attributeBits >>= (READ_BIT(coarseY, 1) << 2);		// Shift 4 if coarse Y bit 1 is set

				// Shift the attribute bits to bit 2 & 3
				attributeBits = (attributeBits & 0x03) << 2;

				// Retrieve the tile slice
				uint8_t tileBuffer[NES_PPU_TILE_SIZE];
				DecodeTileSlice(basePatternTableAddress, patternTableIndex, fineY, &tileBuffer[0]);

				for (uint8_t fineX = 0; fineX < NES_PPU_TILE_SIZE; ++fineX)
				{
					// Set bit 4 when selecting palette index for background tiles 
					uint8_t paletteIndex = tileBuffer[fineX];
					paletteIndex |= attributeBits;

					// Read palette color
					uint16_t paletteAddress = NES_PPU_PALETTE_RAM | paletteIndex;
					uint8_t color = device->videoMemory->ReadU8(paletteAddress);

					// Decode NTSC color to RGB
					DecodeColor(color, buffer);

					buffer += 3;
				}
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