#ifndef _PPU_H_
#define _PPU_H_

#include "environment.h"
#include "nes.h"

#include "Util/Event.h"

namespace libnes
{
	struct Device;

	struct Sprite
	{
		uint8_t y;
		uint8_t tileIdx;
		uint8_t attributes;
		uint8_t x;
	};

	struct PPU_SpriteRegister
	{
		uint8_t tileDataHigh;
		uint8_t tileDataLow;
		uint8_t attributes;
		uint8_t x;
	};

	struct PPU_Registers
	{
		uint16_t tileDataHigh;
		uint16_t tileDataLow;
		uint8_t attributeHigh;
		uint8_t attributeLow;

		uint8_t status;
		uint8_t control;
		uint8_t mask;
		uint8_t oamAddress;

		uint8_t readBuffer;

		uint16_t vramAddress;
		uint16_t temporaryAddress;
		uint8_t fineX;
		bool secondWrite;

		PPU_SpriteRegister sprites[NES_PPU_SECONDARY_OAM_SPRITES];

		bool sprite0ActiveCurrentScanline;
		bool sprite0ActiveNextScanline;
	};

	struct PPU_Latches
	{
		uint8_t nametable;
		uint8_t attribute;
		uint8_t tileLow;
		uint8_t tileHigh;
	};

	class PPU
	{
	private:
		Device * device;
		uint8_t* frameBuffer;

		uint8_t* primaryOAM;
		uint8_t* secondaryOAM;

		uint64_t cycles;
		uint64_t frameFirstCycle;
		uint16_t scanline;
		
		PPU_Registers registers;
		PPU_Latches latches;

		bool nmiState;

	public:
		Event vblankStarted;

	public:
		PPU(Device* device, uint8_t* frameBuffer);
		~PPU();

		void CatchupWhenNMIOcurred();

		void Reset();

		void Catchup(uint64_t masterClockCycle);
		void CatchupToCPU();
		void Tick();

		uint8_t ReadRegister(uint16_t address);
		uint8_t PeekRegister(uint16_t address) const;
		void WriteRegister(uint16_t address, uint8_t value);

		void PerformOAMDMA(uint8_t addressMSB);

		void DecodeColor(uint8_t color, uint8_t* buffer) const;
		void DecodeSprite(const Sprite* sprite, uint8_t* buffer) const;
		void DecodePatternTable(uint16_t address, uint8_t* buffer) const;
		void DecodeNametable(uint16_t address, uint8_t* buffer) const;

		uint64_t Cycles() const
		{
			return cycles;
		}

		uint64_t MasterClockCycles() const
		{
			return cycles * NES_NTSC_PPU_CLOCK_DIVIDER;
		}

		uint16_t Scanline() const
		{
			return scanline;
		}

		const PPU_Registers& GetRegisters() const { return registers; }
		uint8_t* GetOAM() { return primaryOAM; }

	private:
		void EvaluateSprites();
		void FetchSpriteData();
		void ShiftSpriteData();
		uint8_t SelectSprite();

		void IncrementAddress();

		void IncrementCourseX();
		void IncrementY();
		void ResetHorizontal();
		void ResetVertical();

		void FetchBackgroundData();
		void LoadShiftRegisters();
		void ShiftBackgroundData();

		void DrawDot(uint8_t x, uint8_t y, bool background, bool sprites);

		void DecodeTileSlice(uint16_t baseAddress, uint8_t column, uint8_t row, uint8_t y, uint8_t* buffer) const;
		void DecodeTileSlice(uint16_t baseAddress, uint8_t tileIndex, uint8_t y, uint8_t* buffer) const;

		uint8_t ReverseByte(uint8_t n) const;
	};

}

#endif