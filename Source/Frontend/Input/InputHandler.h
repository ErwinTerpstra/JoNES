#ifndef _INPUT_HANDLER_H_
#define _INPUT_HANDLER_H_

namespace libnes
{
	struct Device;
	class StandardController;
}

namespace JoNES
{
	class Window;

	class InputHandler
	{
	private:
		libnes::Device* device;

		libnes::StandardController* p1;

		Window* window;

	public:
		InputHandler(libnes::Device* device, Window* window);
		~InputHandler();

		void Update();
	};
}

#endif