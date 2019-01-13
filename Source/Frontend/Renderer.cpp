#include "stdafx.h"

#include "Renderer.h"

#include "Window.h"

#include <GL/glew.h>

using namespace jones;

Renderer::Renderer(Window& window) : window(window)
{

}

void Renderer::Render()
{
	window.MakeCurrent();
	
	glClear(GL_COLOR_BUFFER_BIT);

	window.Present();
}