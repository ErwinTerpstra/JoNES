#include "Emulator.h"

#include "NES.h"

#include "CPU.h"
#include "Memory/Memory.h"
#include "Memory/MainMemory.h"

#include "Cartridge.h"


using namespace libnes;

Emulator::Emulator()
{
	nes = new NES();
	nes->cpu = new CPU(nes);
	nes->mainMemory = new Memory(*(new MainMemory(nes)));
}

Emulator::~Emulator()
{

}

void Emulator::InsertCartridge(Cartridge* cartridge)
{
	nes->cartridge = cartridge;
}

void Emulator::Update(float time)
{

}