#ifndef _INTERFACE_RENDERER_H_
#define _INTERFACE_RENDERER_H_

#include "Rendering/libgl.h" 

struct ImDrawData;

namespace JoNES
{
	class Window;

	class Program;
	class Shader;

	class InterfaceRenderer
	{
	private:
		Window& window;

		GLuint fontTextureHandle;

		Program* program;
		Shader* vertexShader;
		Shader* fragmentShader;

		unsigned int vboHandle, elementsHandle;

	public:
		InterfaceRenderer(Window& window);

		bool Init();
		void Shutdown();
		void Render();

	private:
		bool InitGraphicsObjects();
		void DestroyGraphicsObjects();
		bool CreateFontTexture();

		void Draw(ImDrawData* draw_data);
	};
}

#endif