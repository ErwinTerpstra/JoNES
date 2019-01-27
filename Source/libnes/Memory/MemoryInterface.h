#ifndef _MEMORY_INTERFACE_H_
#define _MEMORY_INTERFACE_H_

#include "environment.h"

namespace libnes
{
	class MemoryInterface
	{
	public:
		virtual ~MemoryInterface() = 0 { };

		virtual uint8_t Read(uint16_t address) const = 0;
		virtual void Write(uint16_t address, uint8_t value) = 0;
	};

}

#endif