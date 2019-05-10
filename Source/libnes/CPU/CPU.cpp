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
	cycles = 0;

	registers.p = 0x24;	// NOTE: this should be 0x34, but Nintendulator uses 0x24 so this makes comparing logs easier
	registers.a = 0x00;
	registers.x = 0x00;
	registers.y = 0x00;
	registers.s = 0xFD;

	registers.pc = device->mainMemory->ReadU16(NES_RESET_VECTOR);
}

const Instruction& CPU::ExecuteNextInstruction()
{
	// Read the opcode the PC points at
	const Instruction& instruction = DecodeInstruction(registers.pc);

	if (instruction.handler == NULL)
	{
		printf("[CPU]: Missing instruction handler for opcode 0x%02X! at 0x%04X\n", instruction.opcode, registers.pc);
		return instruction;
	}

	uint16_t operandsAddress = registers.pc + 1;

	// Increase the PC to point to the next instruction
	registers.pc += instruction.length();

	// Execute the instruction
	(this->*instruction.handler)(instruction, operandsAddress);
	
	// Increase the clock cycle count
	cycles += instruction.cycleCount;

	CheckInterrupts();

	return instruction;
}

const Instruction& CPU::DecodeInstruction(uint16_t address) const
{
	// Read the opcode
	uint8_t opcode = device->mainMemory->PeekU8(address);

	// Read the instruction description from the instruction map
	return INSTRUCTION_MAP[opcode];
}

void CPU::SetIRQ(bool state)
{
	irq = state;
}

void CPU::TriggerNMI()
{
	nmi = true;
}

void CPU::CheckInterrupts()
{
	if (!registers.GetFlag(FLAG_INTERRUPT_DISABLE) && irq)
		ServiceIRQ();

	if (nmi)
	{
		ServiceNMI();
		nmi = false;
	}
}

void CPU::ServiceIRQ()
{
	PushStackU16(registers.pc);
	PushStackU8(registers.p | 0x20); // IRQ sets bit 5 when pushing

	registers.SetFlag(FLAG_INTERRUPT_DISABLE);

	registers.pc = device->mainMemory->ReadU16(NES_IRQ_VECTOR);

	cycles += NES_CPU_ISR_DURATION;
}

void CPU::ServiceNMI()
{
	PushStackU16(registers.pc);
	PushStackU8(registers.p | 0x20); // IRQ sets bit 5 when pushing

	registers.SetFlag(FLAG_INTERRUPT_DISABLE);

	registers.pc = device->mainMemory->ReadU16(NES_NMI_VECTOR);

	cycles += NES_CPU_ISR_DURATION;
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

Address CPU::ResolveAddress(AddressingModeIdentifier mode, uint16_t operandAddress) const
{
	MemoryBus* memory = device->mainMemory;

	switch (mode)
	{
		case ADDR_IMM:
			return Address(operandAddress);

		case ADDR_ZP:
			return Address(memory->ReadU8(operandAddress));

		case ADDR_ZPX:
			return Address((memory->ReadU8(operandAddress) + registers.x) & 0xFF);

		case ADDR_ZPY:
			return Address((memory->ReadU8(operandAddress) + registers.y) & 0xFF);

		case ADDR_IND:
			return Address(memory->ReadU16_NoPageCross(memory->ReadU16(operandAddress)));

		case ADDR_IZPX:
			return Address(memory->ReadU16_ZeroPage(memory->ReadU8(operandAddress) + registers.x));

		case ADDR_IZPY:
			return Address(memory->ReadU16_ZeroPage(memory->ReadU8(operandAddress)), registers.y);

		case ADDR_ABS:
			return Address(memory->ReadU16(operandAddress));

		case ADDR_ABSX:
			return Address(memory->ReadU16(operandAddress), registers.x);

		case ADDR_ABSY:
			return Address(memory->ReadU16(operandAddress), registers.y);

		case ADDR_REL:
		case ADDR_IMPL:
		case ADDR_INVALID:
		default:
			assert(false);
			return 0xBADA;
	}
}

uint8_t CPU::ReadAddressed(AddressingModeIdentifier mode, uint16_t operandAddress)
{
	Address address = ResolveAddress(mode, operandAddress);

	if (address.crossedPage)
		++cycles;

	return device->mainMemory->ReadU8(address);
}

void CPU::WriteAddressed(AddressingModeIdentifier mode, uint16_t operandAddress, uint8_t value)
{
	Address address = ResolveAddress(mode, operandAddress);

	//if (address.crossedPage)
		//++cycles;

	device->mainMemory->WriteU8(address, value);
}

// ALU instructions
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
	Address address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);
	
	value = ALU::dec(registers, value);

	device->mainMemory->WriteU8(address, value);
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
	Address address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::inc(registers, value);

	device->mainMemory->WriteU8(address, value);
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

	device->mainMemory->WriteU8(address, value);
}

void CPU::rol(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::rol(registers, value);

	device->mainMemory->WriteU8(address, value);
}

void CPU::lsr(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::lsr(registers, value);

	device->mainMemory->WriteU8(address, value);
}

void CPU::ror(const Instruction& instruction, uint16_t operandAddress)
{
	uint16_t address = ResolveAddress(instruction.addressingMode, operandAddress);
	uint8_t value = device->mainMemory->ReadU8(address);

	value = ALU::ror(registers, value);

	device->mainMemory->WriteU8(address, value);
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

// Branch/flag instructions
void CPU::branch(const Instruction& instruction, uint16_t operandAddress)
{
	Flags flag;
	bool branchOnSet = TEST_BIT(instruction.opcode, 5);

	// TODO: Create a look-up table for this
	switch ((instruction.opcode >> 6) & 0x03)
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
	PushStackU8(registers.p | 0x30); // BRK sets bits 4 & 5 when pushing

	registers.SetFlag(FLAG_INTERRUPT_DISABLE);

	registers.pc = device->mainMemory->ReadU16(NES_IRQ_VECTOR);
}

void CPU::rti(const Instruction& instruction, uint16_t operandAddress)
{
	// Clear bit 4 since it doesn't exist in P
	// Set bit 5 because reasons
	registers.p = SET_BIT(UNSET_BIT(PopStackU8(), 4), 5);

	registers.pc = PopStackU16();
}

void CPU::jsr(const Instruction& instruction, uint16_t operandAddress)
{
	PushStackU16(registers.pc - 1);

	registers.pc = device->mainMemory->ReadU16(operandAddress);
}

void CPU::rts(const Instruction& instruction, uint16_t operandAddress)
{
	registers.pc = PopStackU16() + 1;
}

void CPU::jmp_abs(const Instruction& instruction, uint16_t operandAddress)
{
	registers.pc = device->mainMemory->ReadU16(operandAddress);
}

void CPU::jmp_ind(const Instruction& instruction, uint16_t operandAddress)
{
	registers.pc = device->mainMemory->ReadU16_NoPageCross(device->mainMemory->ReadU16(operandAddress));
}

void CPU::bit(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t value = ReadAddressed(instruction.addressingMode, operandAddress);

	registers.SetOrClearFlag(FLAG_ZERO, (registers.a & value) == 0);
	registers.SetOrClearFlag(FLAG_OVERFLOW, TEST_BIT(value, 6));
	registers.SetOrClearFlag(FLAG_NEGATIVE, TEST_BIT(value, 7));
}

void CPU::clc(const Instruction& instruction, uint16_t operandAddress)
{
	registers.ClearFlag(FLAG_CARRY);
}

void CPU::sec(const Instruction& instruction, uint16_t operandAddress)
{
	registers.SetFlag(FLAG_CARRY);
}

void CPU::cld(const Instruction& instruction, uint16_t operandAddress)
{
	registers.ClearFlag(FLAG_DECIMAL_MODE);
}

void CPU::sed(const Instruction& instruction, uint16_t operandAddress)
{
	registers.SetFlag(FLAG_DECIMAL_MODE);
}

void CPU::cli(const Instruction& instruction, uint16_t operandAddress)
{
	registers.ClearFlag(FLAG_INTERRUPT_DISABLE);
}

void CPU::sei(const Instruction& instruction, uint16_t operandAddress)
{
	registers.SetFlag(FLAG_INTERRUPT_DISABLE);
}

void CPU::clv(const Instruction& instruction, uint16_t operandAddress)
{
	registers.ClearFlag(FLAG_OVERFLOW);
}

void CPU::nop(const Instruction& instruction, uint16_t operandAddress)
{

}

// Move instructions
void CPU::lda(const Instruction& instruction, uint16_t operandAddress)
{
	registers.a = ReadAddressed(instruction.addressingMode, operandAddress);
	registers.SetZNFromResult(registers.a);
}

void CPU::sta(const Instruction& instruction, uint16_t operandAddress)
{
	WriteAddressed(instruction.addressingMode, operandAddress, registers.a);
}

void CPU::ldx(const Instruction& instruction, uint16_t operandAddress)
{
	registers.x = ReadAddressed(instruction.addressingMode, operandAddress);
	registers.SetZNFromResult(registers.x);
}

void CPU::stx(const Instruction& instruction, uint16_t operandAddress)
{
	WriteAddressed(instruction.addressingMode, operandAddress, registers.x);
}

void CPU::ldy(const Instruction& instruction, uint16_t operandAddress)
{
	registers.y = ReadAddressed(instruction.addressingMode, operandAddress);
	registers.SetZNFromResult(registers.y);
}

void CPU::sty(const Instruction& instruction, uint16_t operandAddress)
{
	WriteAddressed(instruction.addressingMode, operandAddress, registers.y);
}

void CPU::tax(const Instruction& instruction, uint16_t operandAddress)
{
	registers.x = registers.a;
	registers.SetZNFromResult(registers.x);
}

void CPU::txa(const Instruction& instruction, uint16_t operandAddress)
{
	registers.a = registers.x;
	registers.SetZNFromResult(registers.a);
}

void CPU::tay(const Instruction& instruction, uint16_t operandAddress)
{
	registers.y = registers.a;
	registers.SetZNFromResult(registers.y);
}

void CPU::tya(const Instruction& instruction, uint16_t operandAddress)
{
	registers.a = registers.y;
	registers.SetZNFromResult(registers.a);
}

void CPU::tsx(const Instruction& instruction, uint16_t operandAddress)
{
	registers.x = registers.s;
	registers.SetZNFromResult(registers.x);
}

void CPU::txs(const Instruction& instruction, uint16_t operandAddress)
{
	registers.s = registers.x;
}

void CPU::pla(const Instruction& instruction, uint16_t operandAddress)
{
	registers.a = PopStackU8();
	registers.SetZNFromResult(registers.a);
}

void CPU::pha(const Instruction& instruction, uint16_t operandAddress)
{
	PushStackU8(registers.a);
}

void CPU::plp(const Instruction& instruction, uint16_t operandAddress)
{
	// Clear bit 4 since it doesn't exist in P
	// Set bit 5 because Nintendulator does so
	registers.p = SET_BIT(UNSET_BIT(PopStackU8(), 4), 5);
}

void CPU::php(const Instruction& instruction, uint16_t operandAddress)
{
	// PHP sets bits 4 & 5 when pushing
	PushStackU8(registers.p | 0x30);
}

// Illegal opcodes
void CPU::sax(const Instruction& instruction, uint16_t operandAddress)
{
	WriteAddressed(instruction.addressingMode, operandAddress, registers.a & registers.x);
}

void CPU::lax(const Instruction& instruction, uint16_t operandAddress)
{
	uint8_t value = ReadAddressed(instruction.addressingMode, operandAddress);
	registers.a = value;
	registers.x = value;
	registers.SetZNFromResult(value);
}