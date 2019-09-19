#ifndef _VIDEO_MEMORY_H_
#define _VIDEO_MEMORY_H_

#include "environment.h"

namespace libnes
{
	struct Device;

	class VideoMemory
	{
	private:
		Device* device;

		uint8_t* ram;
		uint8_t* paletteRam;

	public:
		VideoMemory(Device* device);
		~VideoMemory();

		uint8_t ReadU8(uint16_t address);
		uint8_t PeekU8(uint16_t address) const;
		void WriteU8(uint16_t address, uint8_t value);

		uint16_t ReadU16(uint16_t address)
		{
			return ReadU8(address) | (ReadU8(address + 1) << 8); 
		}
		
		void WriteU16(uint16_t address, uint16_t value)
		{
			WriteU8(address + 0, value & 0xFF);
			WriteU8(address + 1, value >> 8);
		}

	private:
		uint16_t MapPaletteAddress(uint16_t address) const;
	};
}

#endif