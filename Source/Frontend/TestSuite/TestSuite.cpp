#include "stdafx.h"

#include "TestSuite.h"
#include "TestMemoryInterface.h"

#include "File.h"

#include "libnes/libnes.h"

using namespace libnes;
using namespace JoNES;

TestSuite::TestSuite(Emulator* emulator) : emulator(emulator)
{
	// Install our memory interface
	emulator->device->mainMemory->Chain<TestMemoryInterface>();
	
	lineBuffer = new char[128];
}

TestSuite::~TestSuite()
{
	if (lineBuffer)
	{
		delete[] lineBuffer;
		lineBuffer = NULL;
	}
}

void TestSuite::RunAutomated(const char* logFileName)
{
	File::DeleteFile(logFileName);

	emulator->device->cpu->registers.pc = 0xC000;

	while (true)
	{
		WriteCurrentStateToLog(logFileName);
		emulator->ExecuteNextInstruction();
	}
}

void TestSuite::WriteCurrentStateToLog(const char* logFileName)
{
	CPU* cpu = emulator->device->cpu;
	MemoryBus* memory = emulator->device->mainMemory;
	Registers& registers = cpu->registers;

	uint16_t pc = registers.pc;
	const Instruction& instruction = cpu->DecodeInstruction(pc);

	char* buffer = lineBuffer;
	const uint32_t bufferSize = 128;

	// PC
	buffer += sprintf_s(buffer, bufferSize, "%04X  ", pc);

	// Operands
	switch (instruction.length())
	{
		case 1:
			buffer += sprintf_s(buffer, bufferSize, "%02X %-7s", memory->ReadU8(pc + 0), "");
			break;

		case 2:
			buffer += sprintf_s(buffer, bufferSize, "%02X %02X %-4s", memory->ReadU8(pc + 0), memory->ReadU8(pc + 1), "");
			break;

		case 3:
			buffer += sprintf_s(buffer, bufferSize, "%02X %02X %02X  ", memory->ReadU8(pc + 0), memory->ReadU8(pc + 1), memory->ReadU8(pc + 2));
			break;
	}

	// Disassembly
	char* dissasemblyStart = buffer;
	buffer += sprintf_s(buffer, bufferSize, "%s ", instruction.assembly);

	switch (instruction.addressingMode)
	{
		case ADDR_IMM:
			buffer += sprintf_s(buffer, bufferSize, "#$%02X", memory->ReadU8(pc + 1));
			break;

		case ADDR_ZP:
			buffer += sprintf_s(buffer, bufferSize, "$%02X", memory->ReadU8(pc + 1));
			break;

		case ADDR_ZPX:
			buffer += sprintf_s(buffer, bufferSize, "$%02X,X", memory->ReadU8(pc + 1));
			break;

		case ADDR_ZPY:
			buffer += sprintf_s(buffer, bufferSize, "$%02X,Y", memory->ReadU8(pc + 1));
			break;

		case ADDR_IND:
			buffer += sprintf_s(buffer, bufferSize, "($%04X)", memory->ReadU16(pc + 1));
			break;

		case ADDR_IZPX:
			buffer += sprintf_s(buffer, bufferSize, "($%02X,X)", memory->ReadU8(pc + 1));
			break;

		case ADDR_IZPY:
			buffer += sprintf_s(buffer, bufferSize, "($%02X),Y", memory->ReadU8(pc + 1));
			break;

		case ADDR_ABS:
			buffer += sprintf_s(buffer, bufferSize, "$%04X", memory->ReadU16(pc + 1));
			break;

		case ADDR_ABSX:
			buffer += sprintf_s(buffer, bufferSize, "$%04X,X", memory->ReadU16(pc + 1));
			break;

		case ADDR_ABSY:
			buffer += sprintf_s(buffer, bufferSize, "$%04X,Y", memory->ReadU16(pc + 1));
			break;

		case ADDR_REL:
			buffer += sprintf_s(buffer, bufferSize, "$%02X", memory->ReadU8(pc + 1));
			break;

		case ADDR_IMPL:
			break;
	}

	// Pad dissasembly
	while (buffer - dissasemblyStart < 32)
		buffer += sprintf_s(buffer, bufferSize, " ");

	// Registers
	buffer += sprintf_s(buffer, bufferSize, "A:%02X X:%02X Y:%02X P:%02X SP:%02X ", 
					  registers.a, registers.x, registers.y, registers.p, registers.s);

	// PPU
	buffer += sprintf_s(buffer, bufferSize, "PPU:%3d,%3d ", 0, 0);

	// Cycles
	buffer += sprintf_s(buffer, bufferSize, "CYC:%llu", cpu->Cycles());

	// Newline
	buffer += sprintf_s(buffer, bufferSize, "\n");

	File::WriteFile(logFileName, lineBuffer, true);
}
