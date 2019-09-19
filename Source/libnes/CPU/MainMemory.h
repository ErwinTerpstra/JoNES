#ifndef _MAIN_MEMORY_H_
#define _MAIN_MEMORY_H_

#include "environment.h"
#include "util.h"


namespace libnes
{
	struct Device;

	class MainMemory
	{
	private:
		Device* device;

		uint8_t* ram;

	public:
		MainMemory(Device* device);
		~MainMemory();

		uint8_t ReadU8(uint16_t address);
		uint8_t PeekU8(uint16_t address) const;
		void WriteU8(uint16_t address, uint8_t value);
		
		int8_t ReadS8(uint16_t address)
		{
			uint8_t value = ReadU8(address);
			return REINTERPRET(value, int8_t);
		}

		uint16_t ReadU16(uint16_t address) 
		{
			return ReadU8(address) | (ReadU8(address + 1) << 8);
		}

		uint16_t ReadU16_ZeroPage(uint8_t offset)
		{
			return ReadU8(offset) | (ReadU8((offset + 1) & 0xFF) << 8);
		}

		uint16_t ReadU16_NoPageCross(uint16_t address)
		{
			uint16_t msbAddress = (address & 0xFF00) | ((address + 1) & 0x00FF);
			return ReadU8(address) | (ReadU8(msbAddress) << 8);
		}
		
		int8_t PeekS8(uint16_t address) const
		{
			uint8_t value = PeekU8(address);
			return REINTERPRET(value, int8_t);
		}

		uint16_t PeekU16(uint16_t address) const
		{
			return PeekU8(address) | (PeekU8(address + 1) << 8);
		}
		
		void WriteU16(uint16_t address, uint16_t value)
		{
			WriteU8(address + 0, value & 0xFF);
			WriteU8(address + 1, value >> 8);
		}

	};
		
}

#endif