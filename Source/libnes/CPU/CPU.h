#ifndef _CPU_H_
#define _CPU_H_

#include "environment.h"
#include "nes.h"

#include "Registers.h"
#include "Instruction.h"

namespace libnes
{
	struct Device;

	struct Address
	{
		uint16_t value;
		bool crossedPage;

		Address(uint16_t value) : value(value), crossedPage(false)
		{

		}

		Address(uint16_t baseValue, uint8_t offset)
		{
			value = baseValue + offset;
			crossedPage = (value & 0xFF00) != (baseValue & 0xFF00);
		}

		operator uint16_t() const
		{
			return value;
		}
	};

	class CPU
	{
	private:
		static const Instruction INSTRUCTION_MAP[256];

		Device* device;

		uint64_t cycles;

		bool irq;
		bool nmi;

	public:
		Registers registers;

	public:
		CPU(Device* device);

		void Reset();

		const Instruction& ExecuteNextInstruction();
		const Instruction& DecodeInstruction(uint16_t address) const;

		void SetIRQ(bool state);
		void TriggerNMI();

		uint64_t Cycles() const
		{
			return cycles;
		}

		uint64_t MasterClockCycles() const
		{
			return cycles * NES_NTSC_CPU_CLOCK_DIVIDER;
		}

		void WaitForOAMDMA()
		{
			if (TEST_BIT(cycles, 0))
				++cycles;

			cycles += 513;
		}

	private:
		void CheckInterrupts();

		void ServiceIRQ();
		void ServiceNMI();

		// Stack management
		void PushStackU8(uint8_t value);
		void PushStackU16(uint16_t value);
		uint8_t PopStackU8();
		uint16_t PopStackU16();

		// Memory addressing
		Address ResolveAddress(AddressingModeIdentifier mode, uint16_t operandAddress) const;
		uint8_t ReadAddressed(AddressingModeIdentifier mode, uint16_t operandAddress);
		void WriteAddressed(AddressingModeIdentifier mode, uint16_t operandAddress, uint8_t value);

		// ALU instructions
		void ora(const Instruction& instruction, uint16_t operandAddress);
		void and$(const Instruction& instruction, uint16_t operandAddress);
		void eor(const Instruction& instruction, uint16_t operandAddress);
		void adc(const Instruction& instruction, uint16_t operandAddress);
		void sbc(const Instruction& instruction, uint16_t operandAddress);
		void cmp(const Instruction& instruction, uint16_t operandAddress);
		void cpx(const Instruction& instruction, uint16_t operandAddress);
		void cpy(const Instruction& instruction, uint16_t operandAddress);
		void dec(const Instruction& instruction, uint16_t operandAddress);
		void dex(const Instruction& instruction, uint16_t operandAddress);
		void dey(const Instruction& instruction, uint16_t operandAddress);
		void inc(const Instruction& instruction, uint16_t operandAddress);
		void inx(const Instruction& instruction, uint16_t operandAddress);
		void iny(const Instruction& instruction, uint16_t operandAddress);
		void asl(const Instruction& instruction, uint16_t operandAddress);
		void rol(const Instruction& instruction, uint16_t operandAddress);
		void lsr(const Instruction& instruction, uint16_t operandAddress);
		void ror(const Instruction& instruction, uint16_t operandAddress);
		void asl_a(const Instruction& instruction, uint16_t operandAddress);
		void rol_a(const Instruction& instruction, uint16_t operandAddress);
		void lsr_a(const Instruction& instruction, uint16_t operandAddress);
		void ror_a(const Instruction& instruction, uint16_t operandAddress);

		// Jump/Flag instructions
		void branch(const Instruction& instruction, uint16_t operandAddress);
		void brk(const Instruction& instruction, uint16_t operandAddress);
		void rti(const Instruction& instruction, uint16_t operandAddress);
		void jsr(const Instruction& instruction, uint16_t operandAddress);
		void rts(const Instruction& instruction, uint16_t operandAddress);
		void jmp_abs(const Instruction& instruction, uint16_t operandAddress);
		void jmp_ind(const Instruction& instruction, uint16_t operandAddress);
		void bit(const Instruction& instruction, uint16_t operandAddress);
		void clc(const Instruction& instruction, uint16_t operandAddress);
		void sec(const Instruction& instruction, uint16_t operandAddress);
		void cld(const Instruction& instruction, uint16_t operandAddress);
		void sed(const Instruction& instruction, uint16_t operandAddress);
		void cli(const Instruction& instruction, uint16_t operandAddress);
		void sei(const Instruction& instruction, uint16_t operandAddress);
		void clv(const Instruction& instruction, uint16_t operandAddress);
		void nop(const Instruction& instruction, uint16_t operandAddress);

		// Move instructions
		void lda(const Instruction& instruction, uint16_t operandAddress);
		void sta(const Instruction& instruction, uint16_t operandAddress);
		void ldx(const Instruction& instruction, uint16_t operandAddress);
		void stx(const Instruction& instruction, uint16_t operandAddress);
		void ldy(const Instruction& instruction, uint16_t operandAddress);
		void sty(const Instruction& instruction, uint16_t operandAddress);
		void tax(const Instruction& instruction, uint16_t operandAddress);
		void txa(const Instruction& instruction, uint16_t operandAddress);
		void tay(const Instruction& instruction, uint16_t operandAddress);
		void tya(const Instruction& instruction, uint16_t operandAddress);
		void tsx(const Instruction& instruction, uint16_t operandAddress);
		void txs(const Instruction& instruction, uint16_t operandAddress);
		void pla(const Instruction& instruction, uint16_t operandAddress);
		void pha(const Instruction& instruction, uint16_t operandAddress);
		void plp(const Instruction& instruction, uint16_t operandAddress);
		void php(const Instruction& instruction, uint16_t operandAddress);

		// Illegal opcodes
		void sax(const Instruction& instruction, uint16_t operandAddress);
		void lax(const Instruction& instruction, uint16_t operandAddress);
	};
}

#endif