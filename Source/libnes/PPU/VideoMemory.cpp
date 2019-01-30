#include "VideoMemory.h"

#include "Device.h"

using namespace libnes;

VideoMemory::VideoMemory(Device* device) : device(device)
{

}

uint8_t VideoMemory::Read(uint16_t address)
{
	return 0;
}

void VideoMemory::Write(uint16_t address, uint8_t value)
{

}