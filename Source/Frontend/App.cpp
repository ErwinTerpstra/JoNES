#include "stdafx.h"
#include "App.h"

#include "libnes/libnes.h"

#include "File.h"

#include "TestSuite/TestSuite.h"

#include "Window.h"

#include "Input/InputHandler.h"

#include "Rendering/Renderer.h"

#include "Interface/InterfaceController.h"
#include "Interface/DebuggerInterface.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"

using namespace JoNES;
using namespace libnes;

App::App()
{

}

bool App::Init()
{
	if (!glfwInit())
		return false;

	time = (float) glfwGetTime();

	emulator = new Emulator();
	debugger = new Debugger(emulator);

	window = Window::Create(1440, 720, "JoNES emulator");

	if (!window)
	{
		Shutdown();
		return false;
	}

	window->SetSwapInterval(0);

	renderer = new Renderer(window, emulator);
	if (!renderer->Init())
	{
		Shutdown();
		return false;
	}

	interfaceController = new InterfaceController(*window);
	if (!interfaceController->Init())
	{
		Shutdown();
		return false;
	}

	inputHandler = new InputHandler(emulator->device, window);

	debuggerInterface = new DebuggerInterface(debugger);
	
	return true;
}

void App::Shutdown()
{
	SAFE_DELETE(inputHandler);

	SAFE_DELETE(debuggerInterface);
	SAFE_DELETE(debugger);
	SAFE_DELETE(emulator);

	if (interfaceController)
	{
		interfaceController->Shutdown();
		delete interfaceController;
	}

	if (renderer)
	{
		renderer->Shutdown();
		delete renderer;
	}

	if (window)
		delete window;

	interfaceController = NULL;
	renderer = NULL;
	window = NULL;

	glfwTerminate();
}

bool App::LoadROM(const char* fileName)
{
	int32_t fileSize = File::GetFileSize(fileName);

	if (fileSize == -1)
		return false;

	uint8_t* buffer = new uint8_t[fileSize];
	int32_t readBytes = File::ReadFile(fileName, buffer, fileSize);

	if (readBytes == -1)
		return false;

	assert(readBytes == fileSize);

	Cartridge* cartridge = new Cartridge();
	cartridge->Load_iNES(buffer, fileSize);

	emulator->InsertCartridge(cartridge);

	return true;
}

bool App::ShouldExit() const
{
	return window->ShouldClose();
}

void App::Update()
{
	glfwPollEvents();

	float currentTime = (float) glfwGetTime();
	deltaTime = currentTime - time;
	time = currentTime;

	inputHandler->Update();

	debugger->Update(time);

	interfaceController->Update(deltaTime);
	debuggerInterface->Update(deltaTime);

	HandleInterface();
}

void App::Render()
{
	renderer->Render();
}

void App::HandleInterface()
{
	ImGui::Begin("Settings");
	
	static bool vsync = false;
	if (ImGui::Checkbox("V-Sync", &vsync))
		window->SetSwapInterval(vsync ? 1 : 0);

	ImGui::End();
}