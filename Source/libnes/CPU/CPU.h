#ifndef _CPU_H_
#define _CPU_H_

#include "environment.h"
#include "nes.h"

#include "Registers.h"

namespace libnes
{
	struct Device;

	class CPU
	{
	public:
		struct Instruction
		{
			uint8_t opcode;
			const char* disassemblyFormat;

			void (CPU::*handler)(uint8_t opcode, uint16_t pc);
		};

	private:
		static const Instruction INSTRUCTION_MAP[256];

		Device* device;

		uint64_t cycles;

	public:
		Registers registers;

	public:
		CPU(Device* device);

		void Reset();

		const Instruction& ExecuteNextInstruction();

		float Time() const
		{
			return cycles / (float) NES_NTSC_CPU_CLOCK_FREQUENCY;
		}

	private:
		uint8_t ReadAddressed(uint8_t opcode, uint16_t pc) const;
		void WriteAddressed(uint8_t opcode, uint16_t pc, uint8_t value);

		void ora(uint8_t opcode, uint16_t pc);
		void anda(uint8_t opcode, uint16_t pc);
		void eor(uint8_t opcode, uint16_t pc);
		void adc(uint8_t opcode, uint16_t pc);
		void sbc(uint8_t opcode, uint16_t pc);
		void cmp(uint8_t opcode, uint16_t pc);
		void cpx(uint8_t opcode, uint16_t pc);
		void cpy(uint8_t opcode, uint16_t pc);


	};
}

#endif