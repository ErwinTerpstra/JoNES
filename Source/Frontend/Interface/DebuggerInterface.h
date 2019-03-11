#ifndef _DEBUGGER_INTERFACE_H_
#define _DEBUGGER_INTERFACE_H_

namespace libnes
{
	class Debugger;
}

namespace JoNES
{
	class Texture;

	class DebuggerInterface
	{
	private:
		libnes::Debugger* debugger;

		Texture* patternTables[2];
		Texture* nametables[4];

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

		void DrawRegisters();
		void DrawDisassembly();
		void DrawBreakpoints();

		void DecodePatternTable(Texture* texture, uint16_t address);
		void DecodeNametable(Texture* texture, uint16_t address);

		bool InputU16(const char* buttonLabel, char* buffer, uint16_t* input);
	};
}

#endif