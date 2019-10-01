#include "stdafx.h"
#include "App.h"

#include "libnes/libnes.h"

#include "File.h"

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

App::App() : vblankEnterred(this, &App::OnVBlankEnterred), emulatorFrameReady(false)
{

}

bool App::Init()
{
	if (!glfwInit())
		return false;

	lastRenderTime = GetTime();

	lastMeasurementRealTime = lastRenderTime;
	lastMeasurementEmulatorTime = 0.0f;

	currentSpeed = 0.0f;
	averageSpeed = 0.0f;

	emulator = new Emulator();
	debugger = new Debugger(emulator);

	emulator->device->ppu->vblankStarted.RegisterEventHandler(&vblankEnterred);
	
	window = Window::Create(1440, 720, "JoNES emulator");

	if (!window)
	{
		Shutdown();
		return false;
	}

	window->SetSwapInterval(0);

	renderer = new Renderer(window, emulator);
	interfaceController = new InterfaceController(*window);

	inputHandler = new InputHandler(emulator->device, window);

	debuggerInterface = new DebuggerInterface(this, debugger);
	
	return true;
}

void App::Shutdown()
{
	SAFE_DELETE(inputHandler);

	SAFE_DELETE(debuggerInterface);
	SAFE_DELETE(debugger);
	SAFE_DELETE(emulator);

	SAFE_DELETE(interfaceController);
	SAFE_DELETE(renderer);

	SAFE_DELETE(window);

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
	float currentTime = GetTime();

	debugger->Update(currentTime);

	Render();
}

void App::Render()
{
	float currentTime = GetTime();
	float deltaTime = currentTime - lastRenderTime;

	// Limit update rate to 60Hz
	if (deltaTime < (1 / 60.0f))
		return;

	glfwPollEvents();

	interfaceController->Update(deltaTime);
	debuggerInterface->Update();

	inputHandler->Update();

	renderer->Render();

	MeasureEmulationSpeed(currentTime);

	lastRenderTime = currentTime;
}

float App::GetTime() const
{
	return (float)glfwGetTime();
}

void App::MeasureEmulationSpeed(float time)
{
	// Measure the emulation speed
	if (time - lastMeasurementRealTime > 1.0f)
	{
		float realDelta = time - lastMeasurementRealTime;
		float emulatorDelta = emulator->Time() - lastMeasurementEmulatorTime;

		currentSpeed = emulatorDelta / realDelta;

		if (averageSpeed > 0.0f)
			averageSpeed = averageSpeed * 0.6f + currentSpeed * 0.4f;
		else
			averageSpeed = currentSpeed;

		lastMeasurementEmulatorTime = emulator->Time();
		lastMeasurementRealTime = time;
	}

}

void App::OnVBlankEnterred()
{
	Render();
}