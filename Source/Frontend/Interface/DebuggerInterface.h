#ifndef _DEBUGGER_INTERFACE_H_
#define _DEBUGGER_INTERFACE_H_

namespace libnes
{
	class Debugger;
}

namespace JoNES
{
	class DebuggerInterface
	{
	private:
		libnes::Debugger* debugger;

	public:
		DebuggerInterface(libnes::Debugger* debugger);

		void Update(float deltaTime);

	private:
		void DrawRegisters();
		void DrawDisassembly();
	};
}

#endif