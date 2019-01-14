#include "CPU.h"

#include "NES.h"
#include "Memory/Memory.h"

#include "debug.h"

using namespace libnes;

CPU::CPU(NES* nes) : nes(nes)
{

}

const CPU::Instruction& CPU::ExecuteNextInstruction()
{
	// Read the opcode the PC points at
	uint8_t opcode = nes->mainMemory->ReadU8(registers.pc);
	
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
	nes->mainMemory->Read(&operandBuffer[0], registers.pc + 1, instruction.length - 1);

	// Increase the PC to point to the next instruction
	registers.pc += instruction.length;

	// Execute the instruction
	(this->*instruction.handler)(opcode, &operandBuffer[0]);

	// Increase the clock cycle count
	//ticks += instruction.duration;

	return instruction;
}