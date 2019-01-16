#include "CPU.h"

#include "Device.h"
#include "Memory/Memory.h"

#include "debug.h"

using namespace libnes;

CPU::CPU(Device* device) : device(device)
{

}

void CPU::Reset()
{
	registers.p = 0x34;
	registers.a = 0x00;
	registers.x = 0x00;
	registers.y = 0x00;
	registers.s = 0xFD;

	registers.pc = 0x0000;
}

const CPU::Instruction& CPU::ExecuteNextInstruction()
{
	// Read the opcode the PC points at
	uint8_t opcode = device->mainMemory->ReadU8(registers.pc);
	
	// Read the instruction description from the instruction map
	const Instruction& instruction = INSTRUCTION_MAP[opcode];

	if (instruction.handler == NULL)
	{
		printf("Missing instruction handler for opcode 0x%02X! at 0x%04X\n", opcode, registers.pc);
		debug::Halt();
		return instruction;
	}

	// Read all operands for this instruction into a small buffer
	uint8_t operandBuffer[4];
	device->mainMemory->Read(&operandBuffer[0], registers.pc + 1, instruction.length - 1);

	// Increase the PC to point to the next instruction
	registers.pc += instruction.length;

	// Execute the instruction
	(this->*instruction.handler)(opcode, &operandBuffer[0]);

	// Increase the clock cycle count
	cycles += instruction.cycles;

	return instruction;
}