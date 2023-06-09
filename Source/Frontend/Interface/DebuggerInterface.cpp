#include "stdafx.h"

#include "DebuggerInterface.h"

#include "App.h"

#include "Rendering/Texture.h"

#include "TestSuite/TestSuite.h"

#include "libnes/libnes.h"
#include "imgui/imgui.h"

using namespace libnes;
using namespace JoNES;

DebuggerInterface::DebuggerInterface(App* app, Debugger* debugger) : app(app), debugger(debugger)
{
	for (uint32_t i = 0; i < 2; ++i)
		patternTables[i] = new Texture(NES_PPU_PATTERN_TABLE_WIDTH, NES_PPU_PATTERN_TABLE_HEIGHT, false);

	for (uint32_t i = 0; i < 4; ++i)
		nametables[i] = new Texture(NES_PPU_NAMETABLE_WIDTH, NES_PPU_NAMETABLE_HEIGHT, false);

	for (uint32_t i = 0; i < NES_PPU_PRIMARY_OAM_SPRITES; ++i)
		sprites[i] = new Texture(NES_PPU_TILE_SIZE, NES_PPU_TILE_SIZE, false);

	textureBuffer = new uint8_t[NES_PPU_PATTERN_TABLE_WIDTH * NES_PPU_PATTERN_TABLE_HEIGHT * 3];
	patternBuffer = new uint8_t[NES_PPU_PATTERN_TABLE_WIDTH * NES_PPU_PATTERN_TABLE_HEIGHT];
	frameBuffer = new uint8_t[NES_FRAME_WIDTH * NES_FRAME_HEIGHT * 3];
}

DebuggerInterface::~DebuggerInterface()
{
	for (uint32_t i = 0; i < 2; ++i)
		SAFE_DELETE(patternTables[i]);

	for (uint32_t i = 0; i < 4; ++i)
		SAFE_DELETE(nametables[i]);

	for (uint32_t i = 0; i < NES_PPU_PRIMARY_OAM_SPRITES; ++i)
		SAFE_DELETE(sprites[i]);

	SAFE_DELETE_ARRAY(textureBuffer);
	SAFE_DELETE_ARRAY(patternBuffer);
	SAFE_DELETE_ARRAY(frameBuffer);
}

void DebuggerInterface::Update()
{
	static bool showEmulatorWindow = true;
	static bool showCPUWindow = true;
	static bool showPPUWindow = true;
	static bool showMemoryWindow = false;

	if (showEmulatorWindow)
		DrawEmulatorWindow(&showEmulatorWindow);

	if (showCPUWindow)
		DrawCPUWindow(&showCPUWindow);

	if (showPPUWindow)
		DrawPPUWindow(&showPPUWindow);

	if (showMemoryWindow)
		DrawMemoryWindow(&showMemoryWindow);
}

void DebuggerInterface::DrawEmulatorWindow(bool* open)
{
	ImGui::Begin("Emulator", open);

	ImGui::Text("Current speed: %4.0f%%; Average speed: %4.0f%%", app->GetCurrentSpeed() * 100, app->GetAverageSpeed() * 100);

	float timeScale = debugger->GetTimeScale();
	if (ImGui::SliderFloat("Time scale", &timeScale, 0.0f, 20.0f, "%.1fx", 2))
		debugger->SetTimeScale(timeScale);

	ImGui::End();
}

void DebuggerInterface::DrawCPUWindow(bool* open)
{
	ImGui::Begin("CPU", open);
	
	ImGui::Columns(2, NULL, false);

	{
		ImGui::Text("State:");
		ImGui::NextColumn();

		ImGui::Text(debugger->IsPaused() ? "paused" : "running");
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	if (debugger->IsPaused())
	{

		if (ImGui::Button("Step"))
		{
			debugger->Step();
			jumpToPC = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Resume"))
			debugger->Resume();
	}
	else
	{
		if (ImGui::Button("Pause"))
		{
			debugger->Pause();
			jumpToPC = true;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Reset"))
	{
		debugger->Reset();
		jumpToPC = true;
	}

	if (ImGui::Button("Run nestest"))
	{
		TestSuite test(debugger->emulator);
		test.RunAutomated("../../Logs/JoNES_nestest.log");
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Registers"))
		DrawRegisters();

	if (ImGui::CollapsingHeader("Disassembly"))
		DrawDisassembly();

	if (ImGui::CollapsingHeader("Breakpoints", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::CollapsingHeader("Execution", ImGuiTreeNodeFlags_DefaultOpen))
			DrawExecutionBreakpoints();

		if (ImGui::CollapsingHeader("Main memory", ImGuiTreeNodeFlags_DefaultOpen))
			DrawMemoryBreakpoints(debugger->mainMemoryBreakpoints);

		if (ImGui::CollapsingHeader("Video memory", ImGuiTreeNodeFlags_DefaultOpen))
			DrawMemoryBreakpoints(debugger->videoMemoryBreakpoints);
	}

	ImGui::End();
}

void DebuggerInterface::DrawPPUWindow(bool* open)
{
	const float patternTableScale = 2;
	const float nametableScale = 1;
	const float spriteScale = 2;

	PPU* ppu = debugger->emulator->device->ppu;

	ImGui::Begin("PPU", open);
	
	if (ImGui::CollapsingHeader("Registers"))
	{
		const PPU_Registers& registers = ppu->GetRegisters();
		
		ImGui::Text("Scanline: %u", ppu->Scanline());
		ImGui::Text("VRAM address: $%04X", registers.vramAddress);

		ImGui::Text("PPUCTRL: $%04X", registers.control);
		ImGui::Text("PPUMASK: $%04X", registers.mask);
		ImGui::Text("PPUSTATUS: $%04X", registers.status);
	}

	if (ImGui::CollapsingHeader("Pattern tables"))
	{
		DecodePatternTable(patternTables[0], NES_PPU_PATTERN0);
		DecodePatternTable(patternTables[1], NES_PPU_PATTERN1);
				
		for (uint32_t i = 0; i < 2; ++i)
		{
			ImGui::Image(patternTables[i], ImVec2(patternTables[i]->Width() * patternTableScale, patternTables[i]->Height() * patternTableScale));
			ImGui::Separator();
		}

	}

	if (ImGui::CollapsingHeader("Nametables"))
	{
		DecodeNametable(nametables[0], NES_PPU_NAMETABLE0);
		DecodeNametable(nametables[1], NES_PPU_NAMETABLE1);
		DecodeNametable(nametables[2], NES_PPU_NAMETABLE2);
		DecodeNametable(nametables[3], NES_PPU_NAMETABLE3);

		for (uint32_t i = 0; i < 4; ++i)
		{
			ImGui::Image(nametables[i], ImVec2(nametables[i]->Width() * nametableScale, nametables[i]->Height() * nametableScale));
			ImGui::Separator();
		}
	}

	if (ImGui::CollapsingHeader("Sprites"))
	{
		uint8_t* oam = ppu->GetOAM();

		for (uint8_t spriteIndex = 0; spriteIndex < NES_PPU_PRIMARY_OAM_SPRITES; ++spriteIndex)
		{
			Sprite* sprite = reinterpret_cast<Sprite*>(oam + spriteIndex * NES_PPU_SPRITE_SIZE);

			DecodeSprite(sprites[spriteIndex], sprite);

			ImGui::Text("#%2u | X: %3u; Y: %3u; Tile: %3u", spriteIndex, sprite->x, sprite->y, sprite->tileIdx);

			ImGui::SameLine();

			ImGui::Image(sprites[spriteIndex], ImVec2(NES_PPU_TILE_SIZE * spriteScale, NES_PPU_TILE_SIZE * spriteScale));

			ImGui::Separator();
		}
	}

	if (ImGui::CollapsingHeader("Palettes"))
	{
		ImGui::Columns(5, NULL, false);

		Device* device = debugger->emulator->device;
		for (uint8_t paletteIndex = 0; paletteIndex < 8; ++paletteIndex)
		{
			ImGui::Text("$%04X:", NES_PPU_PALETTE_RAM | (paletteIndex << 2));
			ImGui::NextColumn();

			for (uint8_t colorIndex = 0; colorIndex < 4; ++colorIndex)
			{
				// Read the color value for this palette and color index
				uint8_t addressLSB = (paletteIndex << 2) + colorIndex;
				uint8_t ntscColor = device->videoMemory->ReadU8(NES_PPU_PALETTE_RAM | addressLSB);

				// Convert the NTSC color to RGB
				uint8_t rgbColor[3];
				device->ppu->DecodeColor(ntscColor, rgbColor);

				ImVec4 color = ImVec4(rgbColor[0] / 255.0f, rgbColor[1] / 255.0f, rgbColor[2] / 255.0f, 1.0f);
				ImGui::Text("$%02X", ntscColor);
				ImGui::SameLine();
				ImGui::ColorButton("", color);

				ImGui::NextColumn();
			}
		}

		ImGui::Columns(1);
	}

	ImGui::End();
}

void DebuggerInterface::DrawMemoryWindow(bool* open)
{
	MainMemory* memoryBus = debugger->emulator->device->mainMemory;

	ImGui::Begin("Memory", open);

	if (debugger->IsPaused() && open)
	{
		for (uint32_t address = 0x0000; address <= 0xFFFF; address += 0x08)
		{
			ImGui::Text("$%04X", address);

			ImGui::SameLine();

			for (uint8_t offset = 0; offset < 8; ++offset)
			{
				ImGui::Text("$%02X", memoryBus->PeekU8(address + offset));

				if (offset < 7)
					ImGui::SameLine();
			}
		}
	}

	ImGui::End();
}

void DebuggerInterface::DecodePatternTable(Texture* texture, uint16_t address)
{
	PPU* ppu = debugger->emulator->device->ppu;

	// Decode the pattern table to palette indices
	ppu->DecodePatternTable(address, patternBuffer);

	// Convert color indices to RGB
	for (int y = 0; y < NES_PPU_PATTERN_TABLE_HEIGHT; ++y)
	{
		for (int x = 0; x < NES_PPU_PATTERN_TABLE_WIDTH; ++x)
		{
			int pixelIndex = y * NES_PPU_PATTERN_TABLE_WIDTH + x;
			
			uint8_t indexedColor = patternBuffer[pixelIndex];
			uint8_t grayscale = indexedColor * 64;
			
			textureBuffer[pixelIndex * 3 + 0] = grayscale;
			textureBuffer[pixelIndex * 3 + 1] = grayscale;
			textureBuffer[pixelIndex * 3 + 2] = grayscale;
		}
	}

	// Load the new RGB data in the texture
	texture->Load(textureBuffer);
}

void DebuggerInterface::DecodeNametable(Texture* texture, uint16_t address)
{
	PPU* ppu = debugger->emulator->device->ppu;

	// Decode the nametable to RGB
	ppu->DecodeNametable(address, frameBuffer);

	// Load the new RGB data in the texture
	texture->Load(frameBuffer);
}

void DebuggerInterface::DecodeSprite(Texture* texture, const Sprite* sprite)
{
	PPU* ppu = debugger->emulator->device->ppu;

	// Decode the nametable to RGB
	ppu->DecodeSprite(sprite, frameBuffer);

	// Load the new RGB data in the texture
	texture->Load(frameBuffer);
}

void DebuggerInterface::DrawRegisters()
{
	CPU* cpu = debugger->emulator->device->cpu;

	// PC, S, A, P, X, Y
	ImGui::Columns(2, NULL, false);

	{
		ImGui::Text("PC: $%04X", cpu->registers.pc);
		
		if (ImGui::BeginPopupContextItem("SetPC"))
		{
			static char pcBuffer[5];
			uint16_t pc;

			if (InputU16("Set", pcBuffer, &pc))
			{
				cpu->registers.pc = pc;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::NextColumn();

		ImGui::Text("S:  $%02X", cpu->registers.s);
		ImGui::NextColumn();

		ImGui::Text("A:  $%02X", cpu->registers.a);
		ImGui::NextColumn();

		ImGui::Text("P:  $%02X", cpu->registers.p);
		ImGui::NextColumn();

		ImGui::Text("X:  $%02X", cpu->registers.x);
		ImGui::NextColumn();

		ImGui::Text("Y:  $%02X", cpu->registers.y);
		ImGui::NextColumn();
	}

	// Flags
	ImGui::Columns(3, NULL, false);
	ImGui::Separator();

	{
		ImGui::Text("C: %u", cpu->registers.GetFlag(FLAG_CARRY));
		ImGui::NextColumn();

		ImGui::Text("Z: %u", cpu->registers.GetFlag(FLAG_ZERO));
		ImGui::NextColumn();

		ImGui::Text("I: %u", cpu->registers.GetFlag(FLAG_INTERRUPT_DISABLE));
		ImGui::NextColumn();

		ImGui::Text("D: %u", cpu->registers.GetFlag(FLAG_DECIMAL_MODE));
		ImGui::NextColumn();
		
		ImGui::Text("V: %u", cpu->registers.GetFlag(FLAG_OVERFLOW));
		ImGui::NextColumn();

		ImGui::Text("N: %u", cpu->registers.GetFlag(FLAG_NEGATIVE));
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}

void DebuggerInterface::DrawDisassembly()
{
	Device* device = debugger->emulator->device;

	CPU* cpu = device->cpu;
	MainMemory* memory = device->mainMemory;
	Registers& registers = cpu->registers;
	
	jumpToPC |= ImGui::Button("Jump to PC");

	ImGui::BeginChild("Disassembly", ImVec2(0, 400), true);
	
	ImGui::Columns(4);
	ImGui::SetColumnWidth(0, 35.0f);
	ImGui::SetColumnWidth(1, 50.0f);
	ImGui::SetColumnWidth(2, 90.0f);

	uint32_t pc = 0x0000;//registers.pc;

	while (pc <= 0xFFFF)
	{
		ImGui::PushID(pc);

		const Instruction& instruction = cpu->DecodeInstruction(pc);

		if (pc == registers.pc)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));

			if (jumpToPC)
			{
				ImGui::SetScrollHereY();
				jumpToPC = false;
			}
		}

		{
			int32_t breakpointIdx = debugger->breakpoints.IndexOf(pc);
			bool hasBreakpoint = breakpointIdx >= 0;

			if (ImGui::Checkbox("", &hasBreakpoint))
			{
				if (!hasBreakpoint)
					debugger->breakpoints.RemoveIndex(breakpointIdx);
				else
					debugger->breakpoints.Add(pc);
			}

			ImGui::NextColumn();
		}

		{
			ImGui::Text("$%04X", pc);
			ImGui::NextColumn();
		}
				
		{
			for (uint32_t i = 0; i < instruction.length(); ++i)
			{
				ImGui::Text("$%02X", memory->PeekU8(pc + i));
				ImGui::SameLine();
			}

			ImGui::NextColumn();
		}

		{
			ImGui::Text("%s", instruction.assembly);
			ImGui::SameLine();

			switch (instruction.addressingMode)
			{
				case ADDR_IMM:
					ImGui::Text("#$%02X", memory->PeekU8(pc + 1));
					break;

				case ADDR_ZP:
					ImGui::Text("$%02X", memory->PeekU8(pc + 1));
					break;

				case ADDR_ZPX:
					ImGui::Text("$%02X,X", memory->PeekU8(pc + 1));
					break;

				case ADDR_ZPY:
					ImGui::Text("$%02X,Y", memory->PeekU8(pc + 1));
					break;

				case ADDR_IND:
					ImGui::Text("($%04X)", memory->PeekU8(pc + 1));
					break;

				case ADDR_IZPX:
					ImGui::Text("($%02X,X)", memory->PeekU8(pc + 1));
					break;

				case ADDR_IZPY:
					ImGui::Text("($%02X),Y", memory->PeekU8(pc + 1));
					break;

				case ADDR_ABS:
					ImGui::Text("$%04X", memory->PeekU16(pc + 1));
					break;

				case ADDR_ABSX:
					ImGui::Text("$%04X,X", memory->PeekU16(pc + 1));
					break;

				case ADDR_ABSY:
					ImGui::Text("$%04X,Y", memory->PeekU16(pc + 1));
					break;

				case ADDR_REL:
					ImGui::Text("$%04X", pc + instruction.length() + memory->PeekS8(pc + 1));
					break;

				case ADDR_IMPL:
					break;
			}

			ImGui::NextColumn();
		}

		if (pc == registers.pc)
			ImGui::PopStyleColor();

		pc += instruction.length();

		ImGui::PopID();
	}

	ImGui::Columns(1);

	ImGui::EndChild();
}

void DebuggerInterface::DrawExecutionBreakpoints()
{
	ImGui::BeginChild("ExecutionBreakpointss", ImVec2(0, 100), true);

	for (uint32_t breakpointIdx = 0; breakpointIdx < debugger->breakpoints.Length(); )
	{
		ImGui::PushID(breakpointIdx);

		ImGui::Text("$%04X", debugger->breakpoints[breakpointIdx]);

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
			debugger->breakpoints.RemoveIndex(breakpointIdx);
		else
			++breakpointIdx;

		ImGui::PopID();
	}

	ImGui::EndChild();

	static char breakpointBuffer[5];
	uint16_t breakpointPC;

	if (InputU16("Add", breakpointBuffer, &breakpointPC))
	{
		debugger->breakpoints.Add(breakpointPC);
			
		breakpointBuffer[0] = '\0';
	}
}

void DebuggerInterface::DrawMemoryBreakpoints(libnes::Vector<libnes::MemoryBreakpoint>& breakpoints)
{
	ImGui::PushID(&breakpoints);

	ImGui::BeginChild("", ImVec2(0, 100), true);

	for (uint32_t breakpointIdx = 0; breakpointIdx < breakpoints.Length(); )
	{
		ImGui::Text("$%04X", breakpoints[breakpointIdx].address);
		ImGui::SameLine();

		ImGui::Checkbox("Read", &breakpoints[breakpointIdx].read);
		ImGui::SameLine();

		ImGui::Checkbox("Write", &breakpoints[breakpointIdx].write);
		ImGui::SameLine();

		if (ImGui::Button("Delete"))
			breakpoints.RemoveIndex(breakpointIdx);
		else
			++breakpointIdx;
	}

	ImGui::EndChild();

	static char breakpointBuffer[5];
	uint16_t breakpointPC;

	if (InputU16("Add", breakpointBuffer, &breakpointPC))
	{
		breakpoints.Add(MemoryBreakpoint { breakpointPC, false, true });

		breakpointBuffer[0] = '\0';
	}

	ImGui::PopID();
}

bool DebuggerInterface::InputU16(const char* buttonLabel, char* buffer, uint16_t* input)
{
	ImGui::InputText("", buffer, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
	ImGui::SameLine();

	if (ImGui::Button(buttonLabel))
	{
		*input = (uint16_t)strtoul(buffer, NULL, 16);
		return true;
	}

	return false;
}