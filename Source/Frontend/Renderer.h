#ifndef _RENDERER_H_
#define _RENDERER_H_

namespace jones
{
	class Window;

	class Renderer
	{
	private:
		Window& window;

	public:
		Renderer(Window& window);

		void Render();
	};
}

#endif