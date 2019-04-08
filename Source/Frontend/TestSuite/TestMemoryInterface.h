#ifndef _TEST_MEMORY_INTERFACE_H_
#define _TEST_MEMORY_INTERFACE_H_

#include "libnes/libnes.h"
#include "Debug.h"

namespace JoNES
{
	class TestMemoryInterface : public libnes::MemoryProxy
	{
	public:

		TestMemoryInterface(libnes::MemoryBus* bus) : MemoryProxy(bus)
		{

		}

		void Write(uint16_t address, uint8_t value)
		{
			if (address == 0x02 || address == 0x03)
				Debug::Print("[nestest]: 0x%4X: 0x%2X\n", address, value);

			MemoryProxy::Write(address, value);
		}


	};
}

#endif