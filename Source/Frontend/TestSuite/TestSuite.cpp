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
}

TestSuite::~TestSuite()
{

}

void TestSuite::RunAutomated(const char* logFileName)
{
	File::DeleteFile(logFileName);

	const uint32_t bufferSize = 1024 * 1024 * 8;
	char* bufferStart = new char[bufferSize];
	char* buffer = bufferStart;

	emulator->device->cpu->registers.pc = 0xC000;

	while (true)
	{
		buffer += WriteCurrentStateToLog(buffer, bufferSize);

		const Instruction& instruction = emulator->ExecuteNextInstruction();

		if (instruction.handler == NULL)
		{
			printf("[nestest]: Stopping automated test because of missing opcode implementation.\n");
			break;
		}
	}

	File::WriteFile(logFileName, bufferStart);
}

uint32_t TestSuite::WriteCurrentStateToLog(char* buffer, uint32_t bufferSize)
{
	CPU* cpu = emulator->device->cpu;
	MemoryBus* memory = emulator->device->mainMemory;
	Registers& registers = cpu->registers;

	uint16_t pc = registers.pc;
	const Instruction& instruction = cpu->DecodeInstruction(pc);

	char* bufferStart = buffer;

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
		{
			uint8_t operand = memory->ReadU8(pc + 1);
			uint8_t value = memory->ReadU8(operand);
			buffer += sprintf_s(buffer, bufferSize, "$%02X = %02X", operand, value);
			break;
		}

		case ADDR_ZPX:
		{
			uint8_t operand = memory->ReadU8(pc + 1);
			uint8_t offset = operand + registers.x;
			uint8_t value = memory->ReadU8(offset);
			buffer += sprintf_s(buffer, bufferSize, "$%02X,X @ %02X = %02X", operand, offset, value);
			break;
		}

		case ADDR_ZPY:
		{
			uint8_t operand = memory->ReadU8(pc + 1);
			uint8_t offset = operand + registers.y;
			uint8_t value = memory->ReadU8(offset);
			buffer += sprintf_s(buffer, bufferSize, "$%02X,Y @ %02X = %02X", operand, offset, value);
			break;
		}

		case ADDR_IND:
		{
			uint16_t operand = memory->ReadU16(pc + 1);
			uint16_t value = memory->ReadU16_NoPageCross(operand);
			buffer += sprintf_s(buffer, bufferSize, "($%04X) = %04X", operand, value);
			break;
		}

		case ADDR_IZPX:
		{
			uint8_t operand = memory->ReadU8(pc + 1);
			uint8_t offset = operand + registers.x;
			uint16_t address = memory->ReadU16_ZeroPage(offset);
			uint8_t value = memory->ReadU8(address);
			buffer += sprintf_s(buffer, bufferSize, "($%02X,X) @ %02X = %04X = %02X", operand, offset, address, value);
			break;
		}

		case ADDR_IZPY:
		{
			uint8_t operand = memory->ReadU8(pc + 1);
			uint16_t address = memory->ReadU16_ZeroPage(operand);
			uint8_t value = memory->ReadU8(address + registers.y);
			buffer += sprintf_s(buffer, bufferSize, "($%02X),Y = %04X @ %04X = %02X", operand, address, address + registers.y, value);
			break;
		}

		case ADDR_ABS:
		{
			uint16_t operand = memory->ReadU16(pc + 1);
			uint8_t value = memory->ReadU8(operand);
			buffer += sprintf_s(buffer, bufferSize, "$%04X = %02X", operand, value);
			break;
		}

		case ADDR_ABSX:
		{
			uint16_t operand = memory->ReadU16(pc + 1);
			uint16_t address = operand + registers.x;
			uint8_t value = memory->ReadU8(operand);
			buffer += sprintf_s(buffer, bufferSize, "$%04X,X @ %04X = %02X", operand, address, value);
			break;
		}

		case ADDR_ABSY:
		{
			uint16_t operand = memory->ReadU16(pc + 1);
			uint16_t address = operand + registers.y;
			uint8_t value = memory->ReadU8(operand);
			buffer += sprintf_s(buffer, bufferSize, "$%04X,Y @ %04X = %02X", operand, address, value);
			break;
		}

		case ADDR_REL:
			buffer += sprintf_s(buffer, bufferSize, "$%04X", pc + instruction.length() + memory->ReadS8(pc + 1));
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

	return buffer - bufferStart;
}
