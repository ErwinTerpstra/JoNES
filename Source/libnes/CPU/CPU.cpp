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

	uint16_t operandsAddress = registers.pc + 1;

	// Increase the PC to point to the next instruction
	uint8_t instructionLength = ADDRESSING_MODE_LENGTHS[instruction.addressingMode];
	registers.pc += instructionLength;

	// Execute the instruction
	(this->*instruction.handler)(instruction, operandsAddress);
	
	// Increase the clock cycle count
	cycles += instruction.cycleCount;

	return instruction;
}

void CPU::PushStackU8(uint8_t value)
{
	device->mainMemory->WriteU8(NES_STACK_START + registers.s, value);
	registers.s -= 1;
}

void CPU::PushStackU16(uint16_t value)
{
	device->mainMemory->WriteU16(NES_STACK_START + registers.s - 1, value);
	registers.s -= 2;
}

uint8_t CPU::PopStackU8()
{
	registers.s += 1;
	return device->mainMemory->ReadU8(NES_STACK_START + registers.s);
}

uint16_t CPU::PopStackU16()
{
	registers.s += 2;
	return device->mainMemory->ReadU16(NES_STACK_START + registers.s - 1);
}

uint16_t CPU::ResolveAddress(AddressingModeIdentifier mode, uint16_t operandAddress) const
{
	MemoryBus* memory = device->mainMemory;

	switch (mode)
	{
		case ADDR_IMM:
			return operandAddress;

		case ADDR_ZP:
			return memory->ReadU8(operandAddress);

		case ADDR_ZPX:
			return memory->ReadU8(operandAddress) + registers.x;

		case ADDR_ZPY:
			return memory->ReadU8(operandAddress) + registers.y;

		case ADDR_IND:
			return memory->ReadU16(memory->ReadU16(operandAddress));

		case ADDR_IZPX:
			return memory->ReadU16(memory->ReadU8(operandAddress) + registers.x);

		case ADDR_IZPY:
			return memory->ReadU16(memory->ReadU8(operandAddress)) + registers.y;

		case ADDR_ABS:
			return memory->ReadU16(operandAddress);

		case ADDR_ABSX:
			return memory->ReadU16(operandAddress) + registers.x;

		case ADDR_ABSY:
			return memory->ReadU16(operandAddress) + registers.y;

		case ADDR_REL:
		case ADDR_IMPL:
		case ADDR_INVALID:
		default:
			assert(false);
			return 0xBADA;
	}
}

uint8_t CPU::ReadAddressed(AddressingModeIdentifier mode, uint16_t operandAddress) const
{
	uint16_t address = ResolveAddress(mode, operandAddress);
	return device->mainMemory->ReadU8(address);
}

void CPU::WriteAddressed(AddressingModeIdentifier mode, uint16_t operandAddress, uint8_t value)
{
	uint16_t address = ResolveAddress(mode, operandAddress);
	device->mainMemory->WriteU8(address, value);
}

void CPU::ora(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, operandAddress);
	ALU::ora(registers, operand);
}

void CPU::and$(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, operandAddress);
	ALU::and$(registers, operand);
}

void CPU::eor(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, operandAddress);
	ALU::eor(registers, operand);
}

void CPU::adc(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, operandAddress);
	ALU::adc(registers, operand);
}

void CPU::sbc(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, operandAddress);
	ALU::sbc(registers, operand);
}

void CPU::cmp(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, operandAddress);
	ALU::cmp(registers, operand);
}

void CPU::cpx(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, operandAddress);
	ALU::cpx(registers, operand);

}
void CPU::cpy(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t operand = ReadAddressed(instruction.addressingMode, operandAddress);
	ALU::cpy(registers, operand);
}

void CPU::dec(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);
	
	value = ALU::dec(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::dex(const Instruction& instruction, uint16_t operandAddress)
{
	ALU::dex(registers);
}

void CPU::dey(const Instruction& instruction, uint16_t operandAddress)
{
	ALU::dey(registers);
}

void CPU::inc(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::inc(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::inx(const Instruction& instruction, uint16_t operandAddress)
{
	ALU::inx(registers);
}

void CPU::iny(const Instruction& instruction, uint16_t operandAddress)
{
	ALU::iny(registers);
}

void CPU::asl(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::asl(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::rol(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::rol(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::lsr(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::lsr(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::ror(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::ror(registers, value);

	device->mainMemory->WriteU16(address, value);
}

void CPU::asl_a(const Instruction& instruction, uint16_t operandAddress)
{
	registers.a = ALU::asl(registers, registers.a);
}

void CPU::rol_a(const Instruction& instruction, uint16_t operandAddress)
{
	registers.a = ALU::rol(registers, registers.a);
}

void CPU::lsr_a(const Instruction& instruction, uint16_t operandAddress)
{
	registers.a = ALU::lsr(registers, registers.a);
}

void CPU::ror_a(const Instruction& instruction, uint16_t operandAddress)
{
	registers.a = ALU::ror(registers, registers.a);
}

void CPU::branch(const Instruction& instruction, uint16_t operandAddress)
{
	Flags flag;
	bool branchOnSet = READ_BIT(instruction.opcode, 5);

	// TODO: Create a look-up table for this
	switch (instruction.opcode >> 5)
	{
	case 0x00:
		flag = FLAG_NEGATIVE;
		break;

	case 0x01:
		flag = FLAG_OVERFLOW;
		break;

	case 0x02:
		flag = FLAG_CARRY;
		break;

	case 0x03:
		flag = FLAG_ZERO;
		break;
	}

	if (registers.GetFlag(flag) == branchOnSet)
	{
		int8_t offset = device->mainMemory->ReadS8(operandAddress);
		registers.pc += offset;

		++cycles;

		// TODO: add cycle on page boundary cross
	}
}

void CPU::brk(const Instruction& instruction, uint16_t operandAddress)
{
	PushStackU16(registers.pc);
	PushStackU8(registers.p);

	registers.SetFlag(FLAG_BREAK);
	registers.SetFlag(FLAG_INTERRUPT_DISABLE);

	registers.pc = device->mainMemory->ReadU16(NES_INTERRUPT_VECTOR);
}

void CPU::rti(const Instruction& instruction, uint16_t operandAddress)
{
	registers.p = PopStackU8();
	registers.pc = PopStackU16();
}

void CPU::jsr(const Instruction& instruction, uint16_t operandAddress)
{
	PushStackU16(registers.pc);

	registers.pc = device->mainMemory->ReadU16(operandAddress);
}

void CPU::rts(const Instruction& instruction, uint16_t operandAddress)
{
	registers.pc = PopStackU16();
}

void CPU::jmp_abs(const Instruction& instruction, uint16_t operandAddress)
{
	registers.pc = device->mainMemory->ReadU16(operandAddress);
}

void CPU::jmp_ind(const Instruction& instruction, uint16_t operandAddress)
{
	registers.pc = device->mainMemory->ReadU16(device->mainMemory->ReadU16(operandAddress));
}