#ifndef _RENDERER_H_
#define _RENDERER_H_

namespace JoNES
{
	class Window;

	class InterfaceRenderer;

	class Renderer
	{
	private:
		Window& window;

		InterfaceRenderer* interfaceRenderer;

	public:
		Renderer(Window& window);

		bool Init();
		void Shutdown();

		void Render();
	};
}

#endif