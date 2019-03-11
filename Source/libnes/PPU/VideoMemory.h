#ifndef _VIDEO_MEMORY_H_
#define _VIDEO_MEMORY_H_

#include "Memory/MemoryInterface.h"

namespace libnes
{
	struct Device;

	class VideoMemory : public MemoryInterface
	{
	private:
		Device* device;

		uint8_t* ram;
		uint8_t* paletteRam;

	public:
		VideoMemory(Device* device);
		~VideoMemory();

		uint8_t Read(uint16_t address);
		void Write(uint16_t address, uint8_t value);

	private:
		uint16_t MapPaletteAddress(uint16_t address) const;
	};
}

#endif