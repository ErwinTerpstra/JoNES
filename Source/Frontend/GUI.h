#ifndef _GUI_H_
#define _GUI_H_

#include "Rendering/libgl.h" 

namespace jones
{
	class Program;
	class Shader;

	class GUI
	{
	private:

		GLuint fontTextureHandle;

		Program* program;
		Shader* vertexShader;
		Shader* fragmentShader;

		unsigned int vboHandle, elementsHandle;

	public:
		GUI();

		bool Init();
		void Shutdown();

	private:
		bool CreateFontTexture();
	};
}

#endif