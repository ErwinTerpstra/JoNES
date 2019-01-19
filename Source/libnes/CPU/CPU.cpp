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

	registers.pc = device->mainMemory->ReadU16(NES_RESET_VECTOR);
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
	(this->*instruction.handler)(instruction, registers.pc + 1);

	// Increase the clock cycle count
	//cycles += instruction.cycles;

	return instruction;
}

uint16_t CPU::ResolveAddress(AddressingMode mode, uint16_t pc) const
{
	MemoryBus* memory = device->mainMemory;

	switch (mode)
	{
		case ADDR_IMM:
			return pc;

		case ADDR_ZP:
			return memory->ReadU8(pc);

		case ADDR_ZPX:
			return memory->ReadU8(pc) + registers.x;

		case ADDR_ZPY:
			return memory->ReadU8(pc) + registers.y;

		case ADDR_IND:
			return memory->ReadU16(pc);

		case ADDR_IZPX:
			return memory->ReadU16(memory->ReadU8(pc) + registers.x);

		case ADDR_IZPY:
			return memory->ReadU16(memory->ReadU8(pc)) + registers.y;

		case ADDR_ABS:
			return memory->ReadU16(pc);

		case ADDR_ABSX:
			return memory->ReadU16(pc) + registers.x;

		case ADDR_ABSY:
			return memory->ReadU16(pc) + registers.y;

		case ADDR_REL:
		case ADDR_IMPL:
		case ADDR_INVALID:
		default:
			assert(false);
			return 0xBADA;
	}
}

uint8_t CPU::ReadAddressed(AddressingMode mode, uint16_t pc) const
{
	uint16_t address = ResolveAddress(mode, pc);
	return device->mainMemory->ReadU8(address);
}

void CPU::WriteAddressed(AddressingMode mode, uint16_t pc, uint8_t value)
{
	uint16_t address = ResolveAddress(mode, pc);
	device->mainMemory->WriteU8(address, value);
}

void CPU::ora(const Instruction& instruction, uint16_t pc)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, pc);
	ALU::ora(registers, operand);
}

void CPU::anda(const Instruction& instruction, uint16_t pc)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, pc);
	ALU::anda(registers, operand);
}

void CPU::eor(const Instruction& instruction, uint16_t pc)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, pc);
	ALU::eor(registers, operand);
}

void CPU::adc(const Instruction& instruction, uint16_t pc)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, pc);
	ALU::adc(registers, operand);
}

void CPU::sbc(const Instruction& instruction, uint16_t pc)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, pc);
	ALU::sbc(registers, operand);
}

void CPU::cmp(const Instruction& instruction, uint16_t pc)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, pc);
	ALU::cmp(registers, operand);
}

void CPU::cpx(const Instruction& instruction, uint16_t pc)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, pc);
	ALU::cpx(registers, operand);

}
void CPU::cpy(const Instruction& instruction, uint16_t pc)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, pc);
	ALU::cpy(registers, operand);
}

void CPU::dec(const Instruction& instruction, uint16_t pc)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, pc);
	uint8_t value = device->mainMemory->ReadU8(address);
	
	value = ALU::dec(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::dex(const Instruction& instruction, uint16_t pc)
{
	ALU::dex(registers);
}

void CPU::dey(const Instruction& instruction, uint16_t pc)
{
	ALU::dey(registers);
}

void CPU::inc(const Instruction& instruction, uint16_t pc)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, pc);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::inc(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::inx(const Instruction& instruction, uint16_t pc)
{
	ALU::inx(registers);
}

void CPU::iny(const Instruction& instruction, uint16_t pc)
{
	ALU::iny(registers);
}

void CPU::asl(const Instruction& instruction, uint16_t pc)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, pc);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::asl(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::rol(const Instruction& instruction, uint16_t pc)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, pc);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::rol(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::lsr(const Instruction& instruction, uint16_t pc)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, pc);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::lsr(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::ror(const Instruction& instruction, uint16_t pc)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, pc);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::ror(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::asl_a(const Instruction& instruction, uint16_t pc)
{
	registers.a = ALU::asl(registers, registers.a);
}

void CPU::rol_a(const Instruction& instruction, uint16_t pc)
{
	registers.a = ALU::rol(registers, registers.a);
}

void CPU::lsr_a(const Instruction& instruction, uint16_t pc)
{
	registers.a = ALU::lsr(registers, registers.a);
}

void CPU::ror_a(const Instruction& instruction, uint16_t pc)
{
	registers.a = ALU::ror(registers, registers.a);
}