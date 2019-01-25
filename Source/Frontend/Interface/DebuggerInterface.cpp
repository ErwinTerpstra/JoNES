#include "stdafx.h"

#include "DebuggerInterface.h"

#include "libnes/libnes.h"
#include "imgui/imgui.h"

using namespace libnes;
using namespace JoNES;

DebuggerInterface::DebuggerInterface(Debugger* debugger) : debugger(debugger)
{

}

void DebuggerInterface::Update(float deltaTime)
{
	ImGui::Begin("Debugger");

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
	
	ImGui::Separator();

	DrawRegisters();

	ImGui::Separator();

	DrawDisassembly();

	ImGui::End();
}

void DebuggerInterface::DrawRegisters()
{
	CPU* cpu = debugger->emulator->device->cpu;

	// PC, S, A, P, X, Y
	ImGui::Columns(2, NULL, false);

	{
		ImGui::Text("PC: 0x%04X", cpu->registers.pc);
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
	ImGui::Columns(4, NULL, false);
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

		ImGui::Text("B: %u", cpu->registers.GetFlag(FLAG_BREAK));
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
		const CPU::Instruction& instruction = cpu->DecodeInstruction(pc);
		
		{
			ImGui::Text("0x%04X", pc);
			ImGui::NextColumn();
		}

		{
			for (uint32_t i = 0; i < CPU::ADDRESSING_MODE_LENGTHS[instruction.addressingMode]; ++i)
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
					ImGui::Text("#0x%02X", memory->ReadU8(pc + 1));
					break;

				case ADDR_ZP:
					ImGui::Text("0x%02X", memory->ReadU8(pc + 1));
					break;

				case ADDR_ZPX:
					ImGui::Text("0x%02X,X", memory->ReadU8(pc + 1));
					break;

				case ADDR_ZPY:
					ImGui::Text("0x%02X,Y", memory->ReadU8(pc + 1));
					break;

				case ADDR_IND:
					ImGui::Text("(0x%04X)", memory->ReadU16(pc + 1));
					break;

				case ADDR_IZPX:
					ImGui::Text("(0x%02X,X)", memory->ReadU8(pc + 1));
					break;

				case ADDR_IZPY:
					ImGui::Text("(0x%02X),Y", memory->ReadU8(pc + 1));
					break;

				case ADDR_ABS:
					ImGui::Text("0x%04X", memory->ReadU16(pc + 1));
					break;

				case ADDR_ABSX:
					ImGui::Text("0x%04X,X", memory->ReadU16(pc + 1));
					break;

				case ADDR_ABSY:
					ImGui::Text("0x%04X,Y", memory->ReadU16(pc + 1));
					break;

				case ADDR_REL:
					ImGui::Text("0x%02X", memory->ReadU8(pc + 1));
					break;

				case ADDR_IMPL:
					break;
			}

			ImGui::NextColumn();
		}

		pc += CPU::ADDRESSING_MODE_LENGTHS[instruction.addressingMode];
	}
}