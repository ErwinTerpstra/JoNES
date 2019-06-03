#include "stdafx.h"

#include "Renderer.h"

#include "Texture.h"
#include "Program.h"
#include "Quad.h"

#include "Window.h"
#include "File.h"

#include "Interface/InterfaceRenderer.h"

#include "libnes/libnes.h"

#include <GL/glew.h>

using namespace JoNES;
using namespace libnes;

Renderer::Renderer(Window* window, Emulator* emulator) : 
	window(window), emulator(emulator), vblankEnterred(this, &Renderer::OnVBlankEnterred), emulatorFrameReady(false)
{
	emulator->device->ppu->vblankStarted.RegisterEventHandler(&vblankEnterred);
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

	char* vertexShaderSource = File::ReadFile("../../Shaders/framebuffer_vert.glsl");
	char* fragmentShaderSource = File::ReadFile("../../Shaders/framebuffer_frag.glsl");

	frameBufferShader = Program::Create(vertexShaderSource, fragmentShaderSource);

	delete vertexShaderSource;
	delete fragmentShaderSource;

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
	glClearColor(0, 0, 0.1f, 0);
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
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	LoadEmulatorFrameBuffer();

	int windowWidth, windowHeight;
	window->GetFrameBufferSize(&windowWidth, &windowHeight);

	int textureWidth = frameBufferTexture->Width();
	int textureHeight = frameBufferTexture->Height();

	frameBufferShader->Bind();
	frameBufferShader->SetFloat("screenRatio", windowWidth / (float)windowHeight);
	frameBufferShader->SetFloat("textureRatio", textureWidth / (float)textureHeight);
	frameBufferShader->BindTexture("mainTexture", GL_TEXTURE0, frameBufferTexture);

	frameBufferQuad->Render(frameBufferShader);
}

void Renderer::LoadEmulatorFrameBuffer()
{
	if (!emulatorFrameReady)
		return;
	
	frameBufferTexture->Load(emulator->frameBuffer);
	emulatorFrameReady = false;
}

void Renderer::OnVBlankEnterred()
{
	emulatorFrameReady = true;
}