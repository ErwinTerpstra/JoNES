#include "stdafx.h"
#include "window.h"

#include <GLFW/glfw3.h>

using namespace jones;

Window::Window(GLFWwindow* handle) : handle(handle)
{

}

Window::~Window()
{
	if (handle)
	{
		glfwDestroyWindow(handle);
		handle = NULL;
	}
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(handle);
}

void Window::MakeCurrent() const
{
	glfwMakeContextCurrent(handle);
}

void Window::Present()
{
	glfwSwapBuffers(handle);
}

Window* Window::Create(int width, int height, const char* title)
{
	GLFWwindow* handle = glfwCreateWindow(width, height, title, NULL, NULL);
	return new Window(handle);
}