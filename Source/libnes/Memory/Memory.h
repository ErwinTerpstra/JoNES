#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "environment.h"

namespace libnes
{
	class MemoryInterface;

	class Memory
	{
	private:
		MemoryInterface& interface;

	public:
		Memory(MemoryInterface& interface);

		uint8_t ReadU8(uint16_t address) const;
		uint16_t ReadU16(uint16_t address) const;
		void Read(uint8_t* buffer, uint16_t address, uint16_t length) const;
		
		void WriteU8(uint16_t address, uint8_t value) const;
		void WriteU16(uint16_t address, uint16_t value) const;
	};
}

#endif