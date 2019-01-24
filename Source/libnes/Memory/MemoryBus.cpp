#include "MemoryBus.h"

#include "MemoryInterface.h"

#include "util.h"

using namespace libnes;

MemoryBus::MemoryBus(MemoryInterface* interface) : interface(interface)
{

}

uint8_t MemoryBus::ReadU8(uint16_t address) const
{
	return interface->Read(address);
}

int8_t MemoryBus::ReadS8(uint16_t address) const
{
	uint8_t value = interface->Read(address);
	return REINTERPRET(value, int8_t);
}

uint16_t MemoryBus::ReadU16(uint16_t address) const
{
	return ReadU8(address) | (ReadU8(address + 1) << 8);
}

void MemoryBus::Read(uint8_t* buffer, uint16_t address, uint16_t length) const
{
	for (uint8_t offset = 0; offset < length; ++offset)
		buffer[offset] = ReadU8(address + offset);
}

void MemoryBus::WriteU8(uint16_t address, uint8_t value) const
{
	interface->Write(address, value);
}

void MemoryBus::WriteU16(uint16_t address, uint16_t value) const
{
	WriteU8(address + 0, value & 0xFF);
	WriteU8(address + 1, value >> 8);
}