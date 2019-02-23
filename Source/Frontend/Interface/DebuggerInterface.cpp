#include "stdafx.h"

#include "DebuggerInterface.h"

#include "Rendering/Texture.h"

#include "TestSuite/TestSuite.h"

#include "libnes/libnes.h"
#include "imgui/imgui.h"

using namespace libnes;
using namespace JoNES;

DebuggerInterface::DebuggerInterface(Debugger* debugger) : debugger(debugger)
{
	patternTableLeft = new Texture(NES_PPU_PATTERN_TABLE_WIDTH, NES_PPU_PATTERN_TABLE_HEIGHT, false);
	patternTableRight = new Texture(NES_PPU_PATTERN_TABLE_WIDTH, NES_PPU_PATTERN_TABLE_HEIGHT, false);

	textureBuffer = new uint8_t[NES_PPU_PATTERN_TABLE_WIDTH * NES_PPU_PATTERN_TABLE_HEIGHT * 3];
	patternBuffer = new uint8_t[NES_PPU_PATTERN_TABLE_WIDTH * NES_PPU_PATTERN_TABLE_HEIGHT];
}

DebuggerInterface::~DebuggerInterface()
{
	SAFE_DELETE_ARRAY(textureBuffer);
	SAFE_DELETE_ARRAY(patternBuffer);

	SAFE_DELETE(patternTableLeft);
	SAFE_DELETE(patternTableRight);
}

void DebuggerInterface::Update(float deltaTime)
{
	static bool showCPUWindow = true;
	static bool showPPUWindow = false;

	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("CPU debugger", NULL, &showCPUWindow);
			ImGui::MenuItem("PPU debugger", NULL, &showPPUWindow);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	DrawCPUWindow(&showCPUWindow);
	DrawPPUWindow(&showPPUWindow);
}

void DebuggerInterface::DrawCPUWindow(bool* open)
{
	ImGui::Begin("CPU debugger", open);
	
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
			debugger->Step();

		ImGui::SameLine();

		if (ImGui::Button("Resume"))
			debugger->Resume();
	}
	else
	{
		if (ImGui::Button("Pause"))
			debugger->Pause();
	}

	ImGui::SameLine();

	if (ImGui::Button("Reset"))
		debugger->Reset();

	if (ImGui::Button("Run nestest"))
	{
		TestSuite test(debugger->emulator);
		test.RunAutomated("../../Logs/JoNES_nestest.log");
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Registers", ImGuiTreeNodeFlags_DefaultOpen))
		DrawRegisters();

	if (ImGui::CollapsingHeader("Disassembly", ImGuiTreeNodeFlags_DefaultOpen))
		DrawDisassembly();

	if (ImGui::CollapsingHeader("Breakpoints", ImGuiTreeNodeFlags_DefaultOpen))
		DrawBreakpoints();

	ImGui::End();
}

void DebuggerInterface::DrawPPUWindow(bool* open)
{
	DecodePatternTable(patternTableLeft, NES_PPU_PATTERN0);
	DecodePatternTable(patternTableRight, NES_PPU_PATTERN1);

	ImGui::Begin("PPU debugger", open);

	{
		ImGui::Text("Pattern tables:");

		const float patternTableScale = 2;
		
		ImGui::Image(patternTableLeft, ImVec2(patternTableLeft->Width() * patternTableScale, patternTableLeft->Height() * patternTableScale));

		ImGui::Separator();

		ImGui::Image(patternTableRight, ImVec2(patternTableRight->Width() * patternTableScale, patternTableRight->Height() * patternTableScale));
	}

	ImGui::End();
}

void DebuggerInterface::DecodePatternTable(Texture* texture, uint16_t address)
{
	PPU* ppu = debugger->emulator->device->ppu;

	// Decode the pattern table
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

			//ppu->DecodeColor(patternBuffer[index], textureBuffer + (index * 3));
		}
	}

	// Load the new RGB data in the texture
	texture->Load(textureBuffer);
}

void DebuggerInterface::DrawRegisters()
{
	CPU* cpu = debugger->emulator->device->cpu;

	// PC, S, A, P, X, Y
	ImGui::Columns(2, NULL, false);

	{
		ImGui::Text("PC: 0x%04X", cpu->registers.pc);
		
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

		ImGui::Text("S:  0x%02X", cpu->registers.s);
		ImGui::NextColumn();

		ImGui::Text("A:  0x%02X", cpu->registers.a);
		ImGui::NextColumn();

		ImGui::Text("P:  0x%02X", cpu->registers.p);
		ImGui::NextColumn();

		ImGui::Text("X:  0x%02X", cpu->registers.x);
		ImGui::NextColumn();

		ImGui::Text("Y:  0x%02X", cpu->registers.y);
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
	MemoryBus* memory = device->mainMemory;
	Registers& registers = cpu->registers;

	uint16_t pc = registers.pc;

	ImGui::Columns(3);

	for (uint32_t i = 0; i < 10; ++i)
	{
		const Instruction& instruction = cpu->DecodeInstruction(pc);
		
		{
			ImGui::Text("0x%04X", pc);
			ImGui::NextColumn();
		}

		{
			for (uint32_t i = 0; i < instruction.length(); ++i)
			{
				ImGui::Text("0x%02X", memory->ReadU8(pc + i));
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
					ImGui::Text("#$%02X", memory->ReadU8(pc + 1));
					break;

				case ADDR_ZP:
					ImGui::Text("$%02X", memory->ReadU8(pc + 1));
					break;

				case ADDR_ZPX:
					ImGui::Text("$%02X,X", memory->ReadU8(pc + 1));
					break;

				case ADDR_ZPY:
					ImGui::Text("$%02X,Y", memory->ReadU8(pc + 1));
					break;

				case ADDR_IND:
					ImGui::Text("($%04X)", memory->ReadU16(pc + 1));
					break;

				case ADDR_IZPX:
					ImGui::Text("($%02X,X)", memory->ReadU8(pc + 1));
					break;

				case ADDR_IZPY:
					ImGui::Text("($%02X),Y", memory->ReadU8(pc + 1));
					break;

				case ADDR_ABS:
					ImGui::Text("$%04X", memory->ReadU16(pc + 1));
					break;

				case ADDR_ABSX:
					ImGui::Text("$%04X,X", memory->ReadU16(pc + 1));
					break;

				case ADDR_ABSY:
					ImGui::Text("$%04X,Y", memory->ReadU16(pc + 1));
					break;

				case ADDR_REL:
					ImGui::Text("$%04X", pc + instruction.length() + memory->ReadS8(pc + 1));
					break;

				case ADDR_IMPL:
					break;
			}

			ImGui::NextColumn();
		}

		pc += instruction.length();
	}

	ImGui::Columns(1);
}

void DebuggerInterface::DrawBreakpoints()
{
	{
		ImGui::BeginChild("breakpoints", ImVec2(0, 100), true);

		for (uint32_t breakpointIdx = 0; breakpointIdx < debugger->breakpoints.Length(); )
		{
			ImGui::Text("$%04X", debugger->breakpoints[breakpointIdx]);

			ImGui::SameLine();

			if (ImGui::Button("Delete"))
				debugger->breakpoints.RemoveIndex(breakpointIdx);
			else
				++breakpointIdx;
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