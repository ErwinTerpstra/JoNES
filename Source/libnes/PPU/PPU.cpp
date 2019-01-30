#include "PPU.h"

#include "Device.h"

using namespace libnes;

PPU::PPU(Device* device) : device(device)
{

}

void PPU::Reset()
{
	cycles = 0;
	scanline = 0;
}

void PPU::Tick()
{
	++cycles;


}