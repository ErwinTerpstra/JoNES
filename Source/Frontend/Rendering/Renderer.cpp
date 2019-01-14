#include "stdafx.h"

#include "Renderer.h"

#include "Window.h"

#include "Interface/InterfaceRenderer.h"

#include <GL/glew.h>

using namespace JoNES;

Renderer::Renderer(Window& window) : window(window)
{

}

bool Renderer::Init()
{
	window.MakeCurrent();

	GLenum glewStatus = glewInit();
	if (glewStatus != GLEW_OK)
		return false;

	interfaceRenderer = new InterfaceRenderer(window);
	interfaceRenderer->Init();
	
	return true;
}

void Renderer::Shutdown()
{
	if (interfaceRenderer)
	{
		interfaceRenderer->Shutdown();
		delete interfaceRenderer;
		interfaceRenderer = NULL;
	}
}

void Renderer::Render()
{	
	window.MakeCurrent();

	// Set viewport and scissor
	int frameBufferWidth, frameBufferHeight;
	window.GetFrameBufferSize(&frameBufferWidth, &frameBufferHeight);
	glViewport(0, 0, frameBufferWidth, frameBufferHeight);
	glScissor(0, 0, frameBufferWidth, frameBufferHeight);

	// Clear frame buffer
	glClearColor(0.25, 0.1f, 0.6f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render interface
	interfaceRenderer->Render();

	// Present the window (swap buffers)
	window.Present();
}