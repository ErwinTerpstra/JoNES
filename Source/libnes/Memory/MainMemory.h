#ifndef _MAIN_MEMORY_H_
#define _MAIN_MEMORY_H_

#include "environment.h"
#include "MemoryInterface.h"

namespace libnes
{
	struct Device;

	class MainMemory : public MemoryInterface
	{
	private:
		Device* device;

		uint8_t* ram;

	public:
		MainMemory(Device* device);
		~MainMemory();

		uint8_t Read(uint16_t address) const;
		void Write(uint16_t address, uint8_t value);

	};
		
}

#endif