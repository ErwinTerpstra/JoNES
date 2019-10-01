#ifndef _DEBUGGER_MEMORY_INTERFACE_H_
#define _DEBUGGER_MEMORY_INTERFACE_H_

#include "environment.h"

#include "Debugger.h"

#include "Util/Vector.h"

namespace libnes
{

	class DebuggerMemoryInterface 
	{
	private:
		Debugger* debugger;
		const Vector<MemoryBreakpoint>& breakpoints;


	public:
		DebuggerMemoryInterface(Debugger* debugger, const Vector<MemoryBreakpoint>& breakpoints) : 
			debugger(debugger), breakpoints(breakpoints)
		{

		}

		uint8_t Read(uint16_t address)
		{
			for (uint32_t breakpointIdx = 0; breakpointIdx < breakpoints.Length(); ++breakpointIdx)
			{
				if (breakpoints[breakpointIdx].read && breakpoints[breakpointIdx].address == address)
				{
					printf("[Debugger]: Memory read breakpoint hit at $%04X\n", address);
					debugger->Pause();
					break;
				}
			}

			//return MemoryProxy::Read(address);
		}

		void Write(uint16_t address, uint8_t value)
		{
			for (uint32_t breakpointIdx = 0; breakpointIdx < breakpoints.Length(); ++breakpointIdx)
			{
				if (breakpoints[breakpointIdx].write && breakpoints[breakpointIdx].address == address)
				{
					printf("[Debugger]: Memory write breakpoint hit at $%04X\n", address);
					debugger->Pause();
					break;
				}
			}

			//MemoryProxy::Write(address, value);
		}
	};

}

#endif