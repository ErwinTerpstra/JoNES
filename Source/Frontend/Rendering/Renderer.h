#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "libnes/libnes.h"

namespace JoNES
{
	class Window;

	class InterfaceRenderer;

	class Texture;
	class Program;
	class Quad;
	
	class Renderer;
	class Renderer
	{
	private:
		Window* window;

		libnes::Emulator* emulator;

		Texture* frameBufferTexture;
		Program* frameBufferShader;
		Quad* frameBufferQuad;

		InterfaceRenderer* interfaceRenderer;

		libnes::EventHandlerProxy<Renderer> vblankEnterred;

	public:
		Renderer(Window* window, libnes::Emulator* emulator);

		bool Init();
		void Shutdown();

		void Render();
		void LoadEmulatorFrameBuffer();

	private:
		void RenderEmulator();

		void OnVBlankEnterred();
	};
}

#endif