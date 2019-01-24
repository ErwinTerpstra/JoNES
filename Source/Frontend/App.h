#ifndef _APP_H_
#define _APP_H_

namespace libnes
{
	class Emulator;
}

namespace JoNES
{
	class Window;
	class Renderer;
	class InterfaceController;
	
	class App
	{
	private:
		float time;
		float deltaTime;

		Window* window;

		Renderer* renderer;
		InterfaceController* interfaceController;
	
	public:
		libnes::Emulator* emulator;

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