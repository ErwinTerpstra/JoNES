#include "stdafx.h"
#include "window.h"

#include <GLFW/glfw3.h>

using namespace JoNES;

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

void Window::SetSwapInterval(int interval) const
{
	MakeCurrent();

	glfwSwapInterval(interval);
}

void Window::Present()
{
	glfwSwapBuffers(handle);
}

void Window::GetWindowSize(int* width, int* height) const
{
	glfwGetWindowSize(handle, width, height);
}

void Window::GetFrameBufferSize(int* width, int* height) const
{
	glfwGetFramebufferSize(handle, width, height);
}


Window* Window::Create(int width, int height, const char* title)
{
	GLFWwindow* handle = glfwCreateWindow(width, height, title, NULL, NULL);
	return new Window(handle);
}