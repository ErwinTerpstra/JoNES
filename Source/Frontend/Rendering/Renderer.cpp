#include "stdafx.h"

#include "Renderer.h"

#include "Texture.h"
#include "Program.h"
#include "Quad.h"

#include "Window.h"

#include "Interface/InterfaceRenderer.h"

#include "libnes/libnes.h"

#include <GL/glew.h>

using namespace JoNES;
using namespace libnes;

Renderer::Renderer(Window* window, Emulator* emulator) : window(window), emulator(emulator)
{

}

bool Renderer::Init()
{
	window->MakeCurrent();

	GLenum glewStatus = glewInit();
	if (glewStatus != GLEW_OK)
		return false;

	interfaceRenderer = new InterfaceRenderer(*window);
	interfaceRenderer->Init();

	frameBufferTexture = new Texture(NES_FRAME_WIDTH, NES_FRAME_HEIGHT, false);
	frameBufferQuad = new Quad();

	const GLchar* vertexShaderSource =
		"layout (location = 0) in vec3 Position;\n"
		"layout (location = 1) in vec2 UV;\n"
		"out vec2 Frag_UV;\n"
		"void main()\n"
		"{\n"
		"    Frag_UV = UV;\n"
		"    gl_Position = vec4(Position.xyz,1);\n"
		"}\n";

	const GLchar* fragmentShaderSource =
		"in vec2 Frag_UV;\n"
		"uniform sampler2D Texture;\n"
		"layout (location = 0) out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"    Out_Color = vec4(Frag_UV.st, 0, 1);//texture(Texture, Frag_UV.st);\n"
		"}\n";

	frameBufferShader = Program::Create(vertexShaderSource, fragmentShaderSource);

	return true;
}

void Renderer::Shutdown()
{
	if (interfaceRenderer)
		interfaceRenderer->Shutdown();

	SAFE_DELETE(interfaceRenderer);

	SAFE_DELETE(frameBufferTexture);
	SAFE_DELETE(frameBufferShader);
	SAFE_DELETE(frameBufferQuad);
}

void Renderer::Render()
{	
	window->MakeCurrent();

	// Set viewport and scissor
	int frameBufferWidth, frameBufferHeight;
	window->GetFrameBufferSize(&frameBufferWidth, &frameBufferHeight);
	glViewport(0, 0, frameBufferWidth, frameBufferHeight);
	glScissor(0, 0, frameBufferWidth, frameBufferHeight);

	// Clear frame buffer
	glClearColor(0, 0, 0.1, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render the emulator frame
	RenderEmulator();

	// Render interface
	interfaceRenderer->Render();

	// Present the window (swap buffers)
	window->Present();
}

void Renderer::RenderEmulator()
{
	frameBufferTexture->Load(emulator->frameBuffer);

	frameBufferShader->Bind();
	frameBufferShader->BindTexture("Texture", GL_TEXTURE0, frameBufferTexture);

	frameBufferQuad->Render(frameBufferShader);
}