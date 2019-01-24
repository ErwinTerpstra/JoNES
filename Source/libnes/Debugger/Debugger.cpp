#include "Debugger.h"

using namespace libnes;

Debugger::Debugger(Emulator* emulator) : emulator(emulator), breakpoints(32), paused(true)
{

}