#include "PPU.h"


#include "Device.h"
#include "CPU/CPU.h"
#include "CPU/MainMemory.h"
#include "Cartridge/Cartridge.h"

#include "VideoMemory.h"

#include "util.h"

using namespace libnes;

const uint8_t NES_2C02_PALETTE_COLORS[] =
{
	84, 84, 84, 0, 30, 116, 8, 16, 144, 48, 0, 136, 68, 0, 100, 92, 0, 48, 84, 4, 0, 60, 24, 0, 32, 42, 0, 8, 58, 0, 0, 64, 0, 0, 60, 0, 0, 50, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	152, 150, 152, 8, 76, 196, 48, 50, 236, 92, 30, 228, 136, 20, 176, 160, 20, 100, 152, 34, 32, 120, 60, 0, 84, 90, 0, 40, 114, 0, 8, 124, 0, 0, 118, 40, 0, 102, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	236, 238, 236, 76, 154, 236, 120, 124, 236, 176, 98, 236, 228, 84, 236, 236, 88, 180, 236, 106, 100, 212, 136, 32, 160, 170, 0, 116, 196, 0, 76, 208, 32, 56, 204, 108, 56, 180, 204, 60, 60, 60, 0, 0, 0, 0, 0, 0,
	236, 238, 236, 168, 204, 236, 188, 188, 236, 212, 178, 236, 236, 174, 236, 236, 174, 212, 236, 180, 176, 228, 196, 144, 204, 210, 120, 180, 222, 120, 168, 226, 144, 152, 226, 180, 160, 214, 228, 160, 162, 160, 0, 0, 0, 0, 0, 0,
};

const uint8_t REVERSED_NIBBLES[] =
{
	0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
	0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf,
};

PPU::PPU(Device* device, uint8_t* frameBuffer) : device(device), frameBuffer(frameBuffer)
{
	primaryOAM = new uint8_t[NES_PPU_PRIMARY_OAM_SIZE];
	secondaryOAM = new uint8_t[NES_PPU_SECONDARY_OAM_SIZE];
}

PPU::~PPU()
{
	SAFE_DELETE_ARRAY(primaryOAM);
	SAFE_DELETE_ARRAY(secondaryOAM);
}


void PPU::Reset()
{
	cycles = 0;
	scanline = 0;
	frameFirstCycle = 0;

	memset(&registers, 0, sizeof(PPU_Registers));
	memset(&latches, 0, sizeof(PPU_Latches));

	nmiState = false;
}

void PPU::CatchupWhenNMIOcurred()
{
	uint64_t nextNmiCycle = frameFirstCycle + NES_PPU_CYCLES_PER_SCANLINE * NES_PPU_VBLANK_FIRST_SCANLINE + 1;
	nextNmiCycle *= NES_NTSC_PPU_CLOCK_DIVIDER;

	if (device->cpu->MasterClockCycles() >= nextNmiCycle)
		CatchupToCPU();
}

void PPU::CatchupToCPU()
{
	Catchup(device->cpu->MasterClockCycles());
}

void PPU::Catchup(uint64_t masterClockCycle)
{
	while (MasterClockCycles() < masterClockCycle)
		Tick();
}

void PPU::Tick()
{
	// Check which dot in the current scanline to update
	uint32_t dot = cycles % NES_PPU_CYCLES_PER_SCANLINE;

	bool col0 = dot <= 8;
	bool drawBackground = TEST_BIT(registers.mask, NES_PPU_MASK_BIT_BACKGROUND) && (!col0 || TEST_BIT(registers.mask, NES_PPU_MASK_BIT_COL0_BACKGROUND));
	bool drawSprites = TEST_BIT(registers.mask, NES_PPU_MASK_BIT_SPRITES) && (!col0 || TEST_BIT(registers.mask, NES_PPU_MASK_BIT_COL0_SPRITES));
	
	if (scanline < NES_FRAME_HEIGHT)
	{
		if (drawBackground || drawSprites)
			HandleFetchAndShift(dot);

		if (drawSprites)
			HandleSprites(dot);

		// Draw dots on visible scanlines
		if (dot >= 1 && dot <= 256)
		{
			DrawDot((uint8_t)(dot - 1), (uint8_t)scanline, drawBackground, drawSprites);
			ShiftSpriteData();
		}

	}
	// Handle VBlank
	else if (scanline == NES_PPU_VBLANK_FIRST_SCANLINE)
	{
		if (dot == 1)
		{
			// Set vblank bit
			registers.status = SET_BIT(registers.status, NES_PPU_STATUS_BIT_VBLANK);
			vblankStarted.Fire();
		}
	}
	// Handle pre-render scanline
	else if (scanline == NES_PPU_PRE_RENDER_SCANLINE)
	{
		if (drawBackground || drawSprites)
			HandleFetchAndShift(dot);

		if (drawSprites)
			HandleSprites(dot);

		// Reset status flags
		if (dot == 1)
			registers.status = 0;

		if (drawBackground || drawSprites)
		{
			// Reset vertical part of VRAM address
			if (dot >= 280 && dot <= 304)
				ResetVertical();
		}

	}

	// Handle NMI
	bool nmiActive = TEST_BIT(registers.status, NES_PPU_STATUS_BIT_VBLANK) && TEST_BIT(registers.control, NES_PPU_CONTROL_BIT_NMI_ENABLE);
	if (nmiActive && !nmiState)
		device->cpu->TriggerNMI();

	nmiState = nmiActive;

	// Increment scanline
	if (dot == (NES_PPU_CYCLES_PER_SCANLINE - 1))
	{
		if (scanline >= NES_PPU_LAST_SCANLINE)
		{
			scanline = 0;
			frameFirstCycle = cycles + 1;
		}
		else
			++scanline;

		registers.sprite0ActiveCurrentScanline = registers.sprite0ActiveNextScanline;
	}

	// Increment cycle count
	++cycles;
}

void PPU::HandleFetchAndShift(uint16_t dot)
{
	if (!(dot >= 2 && dot <= 257) && !(dot >= 322 && dot <= 337))
		return;

	ShiftBackgroundData();

	uint8_t localDot = dot & 0x07;

	// Fetch data from memory at the last dot of each 8-dot cycle
	if (localDot == 0)
	{
		// Load new data into the data latches
		FetchBackgroundData();

		// After fetching, increment course X
		IncrementCourseX();

		// Increment fine Y after the last visible dot
		if (dot == 256)
			IncrementY();
	}
	// Load new data into shift registers at the first dot of each 8-dot cycle
	else if (localDot == 1)
	{
		LoadShiftRegisters();

		// Reset horizontal part of VRAM addresss
		if (dot == 257)
			ResetHorizontal();
	}

}

void PPU::HandleSprites(uint16_t dot)
{
	if (dot == 256)
	{
		// Evaluate sprites on dot 256 (should actually be done during dot 65 - 256)
		if (scanline < NES_FRAME_HEIGHT)
			EvaluateSprites();
	}
	else if (dot == 260)
	{
		// Notify the cartridge hardware of the end of the scanline, this might trigger an interrupt. 
		// Normally the cartridge handles this by listening to the PPU address bus bit 10. This bit change from 0 to 1 when the PPU starts fetching sprite data
		// so this is not entirely accurate
		device->cartridge->CountScanline();
	}
	else if (dot == 320)
	{
		// Fetch sprite tile data and initialize registers (should actually be done during dot 257 - 320)
		FetchSpriteData();
	}
}

uint8_t PPU::ReadRegister(uint16_t address)
{
	CatchupToCPU();

	switch (address & 0x07)
	{
		case 0x02:
		{
			uint8_t value = registers.status;

			// Unset VBlank bit
			registers.status = UNSET_BIT(registers.status, NES_PPU_STATUS_BIT_VBLANK);
			registers.secondWrite = false;
			
			return value;
		}

		case 0x04:
			return primaryOAM[registers.oamAddress];

		case 0x07:
		{
			uint8_t value = device->videoMemory->ReadU8(registers.vramAddress);

			uint8_t result;
			if (address < NES_PPU_PALETTE_RAM)
				result = registers.readBuffer;	// Reads from video memory are placed in a read buffer and are only returned on the next read from this register
			else
				result = value; // Reads from palette RAM are returned immediately

			registers.readBuffer = value;
			IncrementAddress();

			return result;
		}

		default:
			return 0;
	}
}

uint8_t PPU::PeekRegister(uint16_t address) const
{
	switch (address & 0x07)
	{
	case 0x02:
		return registers.status;

	case 0x04:
		return primaryOAM[registers.oamAddress];

	case 0x07:
		return device->videoMemory->ReadU8(registers.vramAddress);

	default:
		return 0;
	}
}

void PPU::WriteRegister(uint16_t address, uint8_t value)
{
	CatchupToCPU();

	switch (address & 0x07)
	{
		case 0x00:
		{
			registers.control = value;

			uint16_t mask = ((value & 0x03) << 10);
			registers.temporaryAddress = (registers.temporaryAddress & ~0x0C00) | mask;
			break;
		}

		case 0x01:
			registers.mask = value;
			break;

		case 0x03:
			registers.oamAddress = value;
			break;

		case 0x04:
			primaryOAM[registers.oamAddress] = value;
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
	CatchupToCPU();

	uint16_t address = addressMSB << 8;

	for (uint16_t offset = 0; offset < NES_PPU_PRIMARY_OAM_SIZE; ++offset)
		primaryOAM[registers.oamAddress + offset] = device->mainMemory->ReadU8(address + offset);
}

void PPU::EvaluateSprites()
{
	// Clear secondary OAM
	memset(&secondaryOAM[0], 0xFF, NES_PPU_SECONDARY_OAM_SIZE);

	uint16_t currentScanline = scanline;
	uint8_t spriteHeight = NES_PPU_TILE_SIZE << (READ_BIT(registers.control, NES_PPU_CONTROL_BIT_SPRITE_HEIGHT));

	uint8_t n = 0;
	uint8_t foundSprites = 0;
	while (n < NES_PPU_PRIMARY_OAM_SPRITES && foundSprites < NES_PPU_SECONDARY_OAM_SPRITES)
	{
		// Get a pointer to the sprite data
		Sprite* sprite = reinterpret_cast<Sprite*>(primaryOAM + (n * NES_PPU_SPRITE_SIZE));

		// Check if the sprite is in range on this scanline
		bool spriteActive = currentScanline >= sprite->y && currentScanline < sprite->y + spriteHeight;
		if (spriteActive)
		{
			// Copy the sprite into secondary oam
			memcpy(secondaryOAM + (foundSprites * NES_PPU_SPRITE_SIZE), sprite, NES_PPU_SPRITE_SIZE);
			++foundSprites;
		}

		if (n == 0)
			registers.sprite0ActiveNextScanline = spriteActive;

		n = n + 1;
	}

	// Continue scanning the OAM for sprites that will not be renderered
	// If they are present, set the sprite overflow flag in the status register
	uint8_t m = 0;
	while (n < NES_PPU_PRIMARY_OAM_SPRITES)
	{
		uint8_t y = primaryOAM[(n * NES_PPU_SPRITE_SIZE) + m];
		if (currentScanline >= y && currentScanline < y + spriteHeight)
			registers.status = SET_BIT(registers.status, NES_PPU_STATUS_BIT_SPRITE_OVERFLOW);
		else
			m = (m + 1) & 0x03; // Sprite overflow bug

		n = (n + 1);
	}

}

void PPU::FetchSpriteData()
{
	bool doubleHeight = READ_BIT(registers.control, NES_PPU_CONTROL_BIT_SPRITE_HEIGHT);

	uint16_t basePatternTableAddress;

	if (!doubleHeight)
		basePatternTableAddress = TEST_BIT(registers.control, NES_PPU_CONTROL_BIT_SPRITE_ADDRESS) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;

	for (uint8_t n = 0; n < NES_PPU_SECONDARY_OAM_SPRITES; ++n)
	{
		Sprite* sprite = reinterpret_cast<Sprite*>(secondaryOAM + n * NES_PPU_SPRITE_SIZE);
		PPU_SpriteRegister& spriteRegister = registers.sprites[n];
		
		// Copy sprite data
		spriteRegister.attributes = sprite->attributes;
		spriteRegister.x = sprite->x;

		// Sprites at Y are either not visible or not selected during sprite evaluation
		if (sprite->y == 0xFF)
		{
			spriteRegister.tileDataLow = 0x00;
			spriteRegister.tileDataHigh = 0x00;
			continue;
		}

		uint8_t fineY = scanline - sprite->y;

		// Handle vertical flipping
		if (TEST_BIT(sprite->attributes, NES_PPU_SPRITE_ATTRIBUTE_BIT_FLIP_Y))
		{
			if (doubleHeight)
				fineY = (NES_PPU_TILE_SIZE << 1) - fineY;
			else
				fineY = NES_PPU_TILE_SIZE - fineY;
		}

		// Determine which tile index should be used
		uint8_t tileIndex;
		if (doubleHeight)
		{
			// For 8x16 sprites, the pattern table is determined by bit 0 of the tile index
			basePatternTableAddress = TEST_BIT(sprite->tileIdx, 0) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;

			// Clear bit 0 of the tile index
			tileIndex = UNSET_BIT(sprite->tileIdx, 0);

			// Select the odd tile index for lower part of 8x16 sprites
			tileIndex |= READ_BIT(fineY, 3);

			fineY &= 0x07;
		}
		else
			tileIndex = sprite->tileIdx;

		// Fetch tile data
		uint16_t address = basePatternTableAddress | (tileIndex << 4) | fineY;
		spriteRegister.tileDataLow = device->videoMemory->ReadU8(address);
		spriteRegister.tileDataHigh = device->videoMemory->ReadU8(address | 0x08);

		// Handle horizontal flipping
		if (TEST_BIT(sprite->attributes, NES_PPU_SPRITE_ATTRIBUTE_BIT_FLIP_X))
		{
			spriteRegister.tileDataLow = ReverseByte(spriteRegister.tileDataLow);
			spriteRegister.tileDataHigh = ReverseByte(spriteRegister.tileDataHigh);
		}
	}
}

void PPU::ShiftSpriteData()
{
	for (uint8_t n = 0; n < NES_PPU_SECONDARY_OAM_SPRITES; ++n)
	{
		PPU_SpriteRegister& spriteRegister = registers.sprites[n];

		if (spriteRegister.x == 0)
		{
			spriteRegister.tileDataHigh <<= 1;
			spriteRegister.tileDataLow <<= 1;
		}
		else
			--spriteRegister.x;
	}
}

uint8_t PPU::SelectSprite()
{
	// Return the index of the first sprite with a non-opaque pixel
	for (uint8_t n = 0; n < NES_PPU_SECONDARY_OAM_SPRITES; ++n)
	{
		PPU_SpriteRegister& spriteRegister = registers.sprites[n];

		if (spriteRegister.x != 0)
			continue;

		// If the tile data is non-zero, the pixel is opaque
		if (READ_BIT(spriteRegister.tileDataLow, 7) != 0 || READ_BIT(spriteRegister.tileDataHigh, 7) != 0)
			return n;
	}

	return NES_PPU_SECONDARY_OAM_SPRITES;
}

void PPU::IncrementAddress()
{
	// Read the increment mode bit to check if we should increment by 32 (one row) or 1 (one column)
	if (TEST_BIT(registers.control, NES_PPU_CONTROL_BIT_INCREMENT_MODE))
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

void PPU::FetchBackgroundData()
{
	// Check which pattern table to use for the background
	uint16_t basePatternTableAddress = TEST_BIT(registers.control, NES_PPU_CONTROL_BIT_BACKGROUND_ADDRESS) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;

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

void PPU::ShiftBackgroundData()
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

void PPU::DrawDot(uint8_t x, uint8_t y, bool background, bool sprites)
{
	// Get background tile data
	uint8_t backgroundTileData = 0;

	if (background)
	{
		uint8_t tileBit = 15 - registers.fineX;

		// Get bit 0 & 1 of the palette index from the top bit in the tile data
		backgroundTileData = READ_BIT(registers.tileDataLow, tileBit) | (READ_BIT(registers.tileDataHigh, tileBit) << 1);
	}

	// Get sprite tile data
	uint8_t spriteTileData = 0;
	uint8_t spritePriority = 1;

	uint8_t spriteIndex = SelectSprite();

	if (sprites && spriteIndex < NES_PPU_SECONDARY_OAM_SPRITES)
	{
		PPU_SpriteRegister& spriteRegister = registers.sprites[spriteIndex];

		// Get bit 0 & 1
		spriteTileData = READ_BIT(spriteRegister.tileDataLow, 7) | (READ_BIT(spriteRegister.tileDataHigh, 7) << 1);

		// Read the priority bit from the sprite attributes
		spritePriority = READ_BIT(spriteRegister.attributes, NES_PPU_SPRITE_ATTRIBUTE_BIT_PRIORITY);

		// Handle sprite 0 hit
		if (spriteIndex == 0 && registers.sprite0ActiveCurrentScanline && spriteTileData != 0 && backgroundTileData != 0)
			registers.status = SET_BIT(registers.status, NES_PPU_STATUS_BIT_SPRITE0_HIT);
	}
		
	// Determine background/sprite priority
	uint8_t paletteIndex = 0;
	if (spriteTileData != 0 && (backgroundTileData == 0 || spritePriority == 0))
	{
		PPU_SpriteRegister& spriteRegister = registers.sprites[spriteIndex];

		// Bit 0 & 1 come from the tile data
		// Bit 2 & 3 come from the sprite attributes
		// Bit 4 is set for sprites
		paletteIndex = spriteTileData | ((spriteRegister.attributes & 0x03) << 2) | 0x10;
	}
	else if (backgroundTileData != 0)
	{
		uint8_t attributeBit = 7 - registers.fineX;

		// Bit 0 & 1 come from the tile data
		// Get bit 2 & 3 of the palette index from the attribute data
		paletteIndex = backgroundTileData | (READ_BIT(registers.attributeLow, attributeBit) << 2) | (READ_BIT(registers.attributeHigh, attributeBit) << 3);
	}
						
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

void PPU::DecodeSprite(const Sprite* sprite, uint8_t* buffer) const 
{
	bool doubleHeight = READ_BIT(registers.control, NES_PPU_CONTROL_BIT_SPRITE_HEIGHT);

	uint8_t spriteHeight;
	uint16_t basePatternTableAddress;

	if (doubleHeight)
	{
		spriteHeight = NES_PPU_TILE_SIZE << 1;

		// For 8x16 sprites, the pattern table is determined by bit 0 of the tile index
		basePatternTableAddress = TEST_BIT(sprite->tileIdx, 0) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;
	}
	else
	{
		spriteHeight = NES_PPU_TILE_SIZE;

		// For 8x8 sprites, the pattern table is determined by a bit of the control register
		basePatternTableAddress = TEST_BIT(registers.control, NES_PPU_CONTROL_BIT_SPRITE_ADDRESS) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;
	}
	
	for (uint8_t y = 0; y < spriteHeight; ++y)
	{
		uint8_t fineY;

		// Handle vertical flipping
		if (TEST_BIT(sprite->attributes, NES_PPU_SPRITE_ATTRIBUTE_BIT_FLIP_Y))
			fineY = spriteHeight - y;
		else
			fineY = y;

		// Determine which tile index should be used
		uint8_t tileIndex;
		if (doubleHeight)
		{
			// Clear bit 0 of the tile index
			tileIndex = UNSET_BIT(sprite->tileIdx, 0);

			// Select the odd tile index for lower part of 8x16 sprites
			tileIndex |= READ_BIT(fineY, 3);

			fineY &= 0x07;
		}
		else
			tileIndex = sprite->tileIdx;
		
		// Fetch tile data
		uint16_t address = basePatternTableAddress | (tileIndex << 4) | fineY;
		uint8_t tileDataLow = device->videoMemory->PeekU8(address);
		uint8_t tileDataHigh = device->videoMemory->ReadU8(address | 0x08);

		// Handle horizontal flipping
		if (TEST_BIT(sprite->attributes, NES_PPU_SPRITE_ATTRIBUTE_BIT_FLIP_X))
		{
			tileDataLow = ReverseByte(tileDataLow);
			tileDataHigh = ReverseByte(tileDataHigh);
		}

		// Decode tile data
		for (uint8_t x = 0; x < NES_PPU_TILE_SIZE; ++x)
		{
			// Pallete index bit 0 & 1 come from tile data
			uint8_t paletteIndex = (READ_BIT(tileDataHigh, 7 - x) << 1) | READ_BIT(tileDataLow, 7 - x);

			// Bit 2 & 3 come from the sprite attributes
			paletteIndex |= (sprite->attributes & 0x03) << 2;

			// Bit 4 is set for sprites
			paletteIndex = SET_BIT(paletteIndex, 4);

			// Read palette color
			uint16_t paletteAddress = NES_PPU_PALETTE_RAM | paletteIndex;
			uint8_t color = device->videoMemory->PeekU8(paletteAddress);

			// Decode NTSC color to RGB
			DecodeColor(color, buffer);

			buffer += 3;
		}
	}
}

void PPU::DecodePatternTable(uint16_t address, uint8_t* buffer) const
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

void PPU::DecodeNametable(uint16_t address, uint8_t* buffer) const
{
	// Check which pattern table to use for the background
	uint16_t basePatternTableAddress = TEST_BIT(registers.control, NES_PPU_CONTROL_BIT_BACKGROUND_ADDRESS) ? NES_PPU_PATTERN1 : NES_PPU_PATTERN0;

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
				uint8_t patternTableIndex = device->videoMemory->PeekU8(nametableAddress);

				// Read the attribute data for this tile
				uint8_t attributeBits = device->videoMemory->PeekU8(attributeAddress);

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
					// Read the tile data
					uint8_t paletteIndex = tileBuffer[fineX];

					// Add the attribute table bits, but only if the tile data is non-zero
					if (paletteIndex != 0)
						paletteIndex |= attributeBits;

					// Read palette color
					uint16_t paletteAddress = NES_PPU_PALETTE_RAM | paletteIndex;
					uint8_t color = device->videoMemory->PeekU8(paletteAddress);

					// Decode NTSC color to RGB
					DecodeColor(color, buffer);

					buffer += 3;
				}
			}
		}
	}
}

void PPU::DecodeTileSlice(uint16_t baseAddress, uint8_t column, uint8_t row, uint8_t y, uint8_t* buffer) const
{
	return DecodeTileSlice(baseAddress, (row << 3) + column, y, buffer);
}

void PPU::DecodeTileSlice(uint16_t baseAddress, uint8_t tileIndex, uint8_t y, uint8_t* buffer) const
{
	uint16_t address = baseAddress | (tileIndex << 4) | (y & 0x07);
	uint8_t lowerPlane = device->videoMemory->ReadU8(address);
	uint8_t upperPlane = device->videoMemory->ReadU8(address | 0x08);

	for (uint8_t x = 0; x < NES_PPU_TILE_SIZE; ++x)
		buffer[x] = (READ_BIT(upperPlane, 7 - x) << 1) | READ_BIT(lowerPlane, 7 - x);
}

uint8_t PPU::ReverseByte(uint8_t n) const
{
	return (REVERSED_NIBBLES[n & 0x0F] << 4) | REVERSED_NIBBLES[n >> 4];
}