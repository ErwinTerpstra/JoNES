#include "stdafx.h"

#include "Frontend.h"

#include "Window.h" 
#include "Renderer.h" 
#include "GUI.h"

using namespace jones;

int main()
{
	Frontend::Init();

	Window* window = Window::Create(640, 480, "JoNES emulator");

	if (!window)
	{
		Frontend::Shutdown();
		return -1;
	}

	Renderer* renderer = new Renderer(*window);

	GUI* gui = new GUI();
	gui->Init();

	/* Loop until the user closes the window */
	while (!window->ShouldClose())
	{
		renderer->Render();

		Frontend::ProcessEvents();
	}

	gui->Shutdown();

	Frontend::Shutdown();

	return 0;
}

