#ifndef _APP_H_
#define _APP_H_

#include "libnes/libnes.h"

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
		float lastRenderTime;

		float currentSpeed;
		float averageSpeed;

		float lastMeasurementRealTime;
		float lastMeasurementEmulatorTime;

		Window* window;
		InputHandler* inputHandler;

		Renderer* renderer;
		InterfaceController* interfaceController;
		DebuggerInterface* debuggerInterface;

		libnes::EventHandlerProxy<App> vblankEnterred;
		bool emulatorFrameReady;
			
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
		
		float GetTime() const;
		float GetCurrentSpeed() const { return currentSpeed; }
		float GetAverageSpeed() const { return averageSpeed; }

	private:
		void MeasureEmulationSpeed(float time);
		void OnVBlankEnterred();
	};
}

#endif