#ifndef _VIDEO_MEMORY_H_
#define _VIDEO_MEMORY_H_

#include "Memory/MemoryInterface.h"

namespace libnes
{
	class VideoMemory : public MemoryInterface
	{

	public:
		VideoMemory();

		uint8_t Read(uint16_t address);
		void Write(uint16_t address, uint8_t value);
	};
}

#endif