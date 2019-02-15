#ifndef _RENDERER_H_
#define _RENDERER_H_

namespace libnes
{
	class Emulator;
}

namespace JoNES
{
	class Window;

	class InterfaceRenderer;

	class Texture;
	class Program;
	class Quad;

	class Renderer
	{
	private:
		Window* window;

		libnes::Emulator* emulator;

		Texture* frameBufferTexture;
		Program* frameBufferShader;
		Quad* frameBufferQuad;

		InterfaceRenderer* interfaceRenderer;

	public:
		Renderer(Window* window, libnes::Emulator* emulator);

		bool Init();
		void Shutdown();

		void Render();

	private:
		void RenderEmulator();
	};
}

#endif