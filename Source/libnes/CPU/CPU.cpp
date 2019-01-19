#include "CPU.h"

#include "ALU.h"

#include "Device.h"
#include "Memory/MemoryBus.h"

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
	
	// Increase the PC to point to the next instruction
	//registers.pc += instruction.length;

	// Execute the instruction
	(this->*instruction.handler)(opcode, registers.pc + 1);

	// Increase the clock cycle count
	//cycles += instruction.cycles;

	return instruction;
}

uint8_t CPU::ReadAddressed(uint8_t opcode, uint16_t pc) const
{
	MemoryBus* memory = device->mainMemory;

	switch (opcode & 0x0F)
	{
		case 0x00:
			assert(opcode == 0xA0 || opcode == 0xC0 || opcode == 0xE0);
			return memory->ReadU8(pc); // OPC #
			break;

		case 0x01:
			if ((opcode & 0x10) == 0x10)
				return memory->ReadU8(memory->ReadU16(memory->ReadU8(pc)) + registers.y); // OPC ($LL),Y
			else
				return memory->ReadU8(memory->ReadU16(memory->ReadU8(pc) + registers.x)); // OPC ($LL,X)

		case 0x02:
			assert(opcode == 0xA2);
			return memory->ReadU8(pc); // OPC #

		case 0x03:
			assert(false);
			break;

		case 0x04:
			if ((opcode & 0x20) == 0x20)
				return memory->ReadU8(memory->ReadU8(pc)); // OPC $LL
			else
				return memory->ReadU8(memory->ReadU8(pc) + registers.x); // OPC $LL,X

		case 0x05:
			if ((opcode & 0x10) == 0x10)
				return memory->ReadU8(memory->ReadU8(pc) + registers.x); // OPC $LL,X
			else
				return memory->ReadU8(memory->ReadU8(pc)); // OPC $LL

		case 0x06:
			if (opcode == 0x96 || opcode == 0xB6)
				return memory->ReadU8(memory->ReadU8(pc) + registers.y); // OPC $LL,Y
			if ((opcode & 0x10) == 0x10)
				return memory->ReadU8(memory->ReadU8(pc) + registers.x); // OPC $LL,X
			else
				return memory->ReadU8(memory->ReadU8(pc)); // OPC $LL

		case 0x07:
			assert(false);
			break;

		case 0x08:
			assert(false);
			break;

		case 0x09:
			assert(opcode != 0x99);

			if ((opcode & 0x10) == 0x10)
				return memory->ReadU8(memory->ReadU16(pc) + registers.y); // OPC $LLHH,Y
			else
				return memory->ReadU8(pc); // OPC #

		case 0x0A:
			assert((opcode & 0x10) == 0x00 && (opcode & 0x80) == 0x00);
			return registers.a;

		case 0x0B:
			assert(false);
			break;

		case 0x0C:
			switch (opcode & 0xF0)
			{
				case 0x20:
				case 0x40:
				case 0x80:
				case 0xA0:
				case 0xC0:
				case 0xE0:
					return memory->ReadU8(memory->ReadU16(pc)); // OPC $LLHH

				case 0x60:
					assert(false);
					break;

				case 0xB0:
					return memory->ReadU8(memory->ReadU16(pc) + registers.x); // OPC $LLHH,X
			}
			break;

		case 0x0D:
			if ((opcode & 0x10) == 0x10)
				return memory->ReadU8(memory->ReadU16(pc) + registers.x); // OPC $LLHH, X
			else
				return memory->ReadU8(memory->ReadU16(pc)); // OPC $LLHH

		case 0x0E:
			assert(opcode != 0x90);

			if (opcode == 0xBE)
				return memory->ReadU8(memory->ReadU16(pc) + registers.y); // OPC $LLHH,Y
			if ((opcode & 0x10) == 0x10)
				return memory->ReadU8(memory->ReadU16(pc) + registers.x); // OPC $LLHH, X
			else
				return memory->ReadU8(memory->ReadU16(pc)); // OPC $LLHH

		case 0x0F:
			assert(false);
			break;
	}
}

void CPU::WriteAddressed(uint8_t opcode, uint16_t pc, uint8_t value)
{

}

void CPU::ora(uint8_t opcode, uint16_t pc)
{
	uint8_t operand = ReadAddressed(opcode, pc);
	ALU::ora(registers, operand);
}

void CPU::anda(uint8_t opcode, uint16_t pc)
{
	uint8_t operand = ReadAddressed(opcode, pc);
	ALU::and_(registers, operand);
}

void CPU::eor(uint8_t opcode, uint16_t pc)
{
	uint8_t operand = ReadAddressed(opcode, pc);
	ALU::eor(registers, operand);
}

void CPU::adc(uint8_t opcode, uint16_t pc)
{
	uint8_t operand = ReadAddressed(opcode, pc);
	ALU::adc(registers, operand);
}

void CPU::sbc(uint8_t opcode, uint16_t pc)
{
	uint8_t operand = ReadAddressed(opcode, pc);
	ALU::sbc(registers, operand);
}

void CPU::cmp(uint8_t opcode, uint16_t pc)
{
	uint8_t operand = ReadAddressed(opcode, pc);
	ALU::cmp(registers, operand);
}

void CPU::cpx(uint8_t opcode, uint16_t pc)
{
	uint8_t operand = ReadAddressed(opcode, pc);
	ALU::cpx(registers, operand);

}
void CPU::cpy(uint8_t opcode, uint16_t pc)
{
	uint8_t operand = ReadAddressed(opcode, pc);
	ALU::cpy(registers, operand);
}