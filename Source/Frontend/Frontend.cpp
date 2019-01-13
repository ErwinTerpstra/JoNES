#include "stdafx.h"
#include "Frontend.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace jones;

bool Frontend::Init()
{
	if (glfwInit() != 0)
		return false;

	if (glewInit() != GLEW_OK)
		return false;

	return true;
}

void Frontend::Shutdown()
{
	glfwTerminate();
}

void Frontend::ProcessEvents()
{
	glfwPollEvents();
}