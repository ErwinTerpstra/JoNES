#ifndef _APP_H_
#define _APP_H_

namespace libnes
{
	class Emulator;
	class Debugger;
}

namespace JoNES
{
	class Window;
	class InputHandler;

	class Renderer;
	class InterfaceController;
	class DebuggerInterface;
	
	class App
	{
	private:
		float time;
		float deltaTime;

		Window* window;
		InputHandler* inputHandler;

		Renderer* renderer;
		InterfaceController* interfaceController;
		DebuggerInterface* debuggerInterface;
			
	public:
		libnes::Emulator* emulator;
		libnes::Debugger* debugger;

	public:
		App();

		bool Init();
		void Shutdown();

		bool ShouldExit() const;

		bool LoadROM(const char* fileName);

		void Update();
		void Render();

	private:
		void HandleInterface();
	};
}

#endif