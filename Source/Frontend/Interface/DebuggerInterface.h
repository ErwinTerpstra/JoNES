#ifndef _DEBUGGER_INTERFACE_H_
#define _DEBUGGER_INTERFACE_H_

#include "libnes/libnes.h"

namespace JoNES
{
	class Texture;

	class DebuggerInterface
	{
	private:
		libnes::Debugger* debugger;

		Texture* patternTables[2];
		Texture* nametables[4];
		Texture* sprites[NES_PPU_PRIMARY_OAM_SPRITES];

		uint8_t* textureBuffer;
		uint8_t* patternBuffer;
		uint8_t* frameBuffer;

	public:
		DebuggerInterface(libnes::Debugger* debugger);
		~DebuggerInterface();

		void Update(float deltaTime);

	private:
		void DrawCPUWindow(bool* open);
		void DrawPPUWindow(bool* open);
		void DrawMemoryWindow(bool* open);

		void DrawRegisters();
		void DrawDisassembly();
		void DrawExecutionBreakpoints();
		void DrawMemoryBreakpoints(libnes::Vector<libnes::MemoryBreakpoint>& breakpoints);

		void DecodePatternTable(Texture* texture, uint16_t address);
		void DecodeNametable(Texture* texture, uint16_t address);
		void DecodeSprite(Texture* texture, const libnes::Sprite* sprite);

		bool InputU16(const char* buttonLabel, char* buffer, uint16_t* input);
	};
}

#endif