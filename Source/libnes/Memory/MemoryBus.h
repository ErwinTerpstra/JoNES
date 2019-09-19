#ifndef _MEMORY_BUS_H_
#define _MEMORY_BUS_H_

#include "environment.h"
#include "debug.h"

#include "MemoryInterface.h"

namespace libnes
{
	class MemoryBus
	{
	private:
		MemoryInterface* interface;

	public:
		MemoryBus(MemoryInterface* interface);
		
		void BindInterface(MemoryInterface* interface)
		{
			this->interface = interface;
		}

		MemoryInterface* GetInterface() const { return interface; }

		uint8_t ReadU8(uint16_t address) const;
		uint8_t PeekU8(uint16_t address) const;

		int8_t ReadS8(uint16_t address) const;
		int8_t PeekS8(uint16_t address) const;
		
		uint16_t ReadU16(uint16_t address) const;
		uint16_t ReadU16_NoPageCross(uint16_t address) const;
		uint16_t ReadU16_ZeroPage(uint8_t offset) const;
		uint16_t PeekU16(uint16_t address) const;

		void WriteU8(uint16_t address, uint8_t value) const;
		void WriteU16(uint16_t address, uint16_t value) const;
	};
}

#endif