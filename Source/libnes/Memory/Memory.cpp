#include "Memory.h"

#include "MemoryInterface.h"

using namespace libnes;

Memory::Memory(MemoryInterface& interface) : interface(interface)
{

}

uint8_t Memory::ReadU8(uint16_t address) const
{
	return interface.Read(address);
}

uint16_t Memory::ReadU16(uint16_t address) const
{
	return ReadU8(address) | (ReadU8(address + 1) << 8);
}

void Memory::Read(uint8_t* buffer, uint16_t address, uint16_t length) const
{
	for (uint8_t offset = 0; offset < length; ++offset)
		buffer[offset] = ReadU8(address + offset);
}

void Memory::WriteU8(uint16_t address, uint8_t value) const
{
	interface.Write(address, value);
}

void Memory::WriteU16(uint16_t address, uint16_t value) const
{
	WriteU8(address + 0, value & 0xFF);
	WriteU8(address + 1, value >> 8);
}