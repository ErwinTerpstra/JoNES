#ifndef _MEMORY_INTERFACE_H_
#define _MEMORY_INTERFACE_H_

#include "environment.h"

namespace libnes
{
	struct MemoryInterface
	{
		typedef uint8_t(*Read)(uint16_t address);
		typedef uint8_t(*Peek)(uint16_t address);

		typedef void (*Write)(uint16_t address, uint8_t value);

		Peek peek;
		Read read;
		Write write;
	};

}

#endif