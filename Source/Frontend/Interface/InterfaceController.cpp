#include "stdafx.h"

#include "InterfaceController.h"

#include "Window.h"

#include "imgui/imgui.h"

#include <GLFW/glfw3.h>

#define GLFWCallback(functionName)\
		[](GLFWwindow* window, auto... args)\
		{\
            auto pointer = static_cast<InterfaceController*>(glfwGetWindowUserPointer(window));\
            pointer->functionName(window, args...);\
        }

using namespace JoNES;

InterfaceController::InterfaceController(Window& window) : window(window)
{
	for (int i = 0; i < IM_ARRAYSIZE(mouseDown); ++i)
		mouseDown[i] = false;

	for (int i = 0; i < IM_ARRAYSIZE(mouseCursors); ++i)
		mouseCursors[i] = 0;

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
	
	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

#if defined(_WIN32) && defined(GLFW_EXPOSE_NATIVE_WIN32)
	io.ImeWindowHandle = (void*)glfwGetWin32Window(window.GetHandle());
#endif

	// Create mouse cursors
	mouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	mouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	mouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	mouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	mouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
	
	// Setup callbacks
	glfwSetWindowUserPointer(window.GetHandle(), this);

	glfwSetMouseButtonCallback(window.GetHandle(), GLFWCallback(MouseButtonCallback));
	glfwSetScrollCallback(window.GetHandle(), GLFWCallback(ScrollCallback));
	glfwSetKeyCallback(window.GetHandle(), GLFWCallback(KeyCallback));
	glfwSetCharCallback(window.GetHandle(), GLFWCallback(CharCallback));

}

InterfaceController::~InterfaceController()
{
	for (ImGuiMouseCursor cursorIdx = 0; cursorIdx < ImGuiMouseCursor_COUNT; cursorIdx++)
	{
		glfwDestroyCursor(mouseCursors[cursorIdx]);
		mouseCursors[cursorIdx] = NULL;
	}
}

void InterfaceController::Update(float dt)
{
	ImGuiIO& io = ImGui::GetIO();

	// Setup display size and frame buffer scale
	int windowWidth, windowHeight;
	int bufferWidth, bufferHeight;
	window.GetWindowSize(&windowWidth, &windowHeight);
	window.GetFrameBufferSize(&bufferWidth, &bufferHeight);

	io.DisplaySize = ImVec2((float)windowWidth, (float)windowHeight);
	io.DisplayFramebufferScale = ImVec2(windowWidth > 0 ? ((float)bufferWidth / windowWidth) : 0,
		windowHeight > 0 ? ((float)bufferHeight / windowHeight) : 0);

	// Setup time step
	io.DeltaTime = dt;

	UpdateMouse();
	UpdateCursor();

	ImGui::NewFrame();
}

void InterfaceController::UpdateMouse()
{
	// Update buttons
	ImGuiIO& io = ImGui::GetIO();
	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
	{
		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		io.MouseDown[i] = mouseDown[i] || glfwGetMouseButton(window.GetHandle(), i) != 0;
		mouseDown[i] = false;
	}

	// Update mouse position
	const bool focused = glfwGetWindowAttrib(window.GetHandle(), GLFW_FOCUSED) != 0;

	if (focused)
	{
		if (io.WantSetMousePos)
		{
			glfwSetCursorPos(window.GetHandle(), (double)io.MousePos.x, (double)io.MousePos.y);
		}
		else
		{
			double mouseX, mouseY;
			glfwGetCursorPos(window.GetHandle(), &mouseX, &mouseY);
			io.MousePos = ImVec2((float)mouseX, (float)mouseY);
		}
	}
	else
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
}

void InterfaceController::UpdateCursor()
{
	ImGuiIO& io = ImGui::GetIO();
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(window.GetHandle(), GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		return;

	ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
	if (cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
	{
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		glfwSetInputMode(window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else
	{
		// Show OS mouse cursor
		// FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
		glfwSetCursor(window.GetHandle(), mouseCursors[cursor] ? mouseCursors[cursor] : mouseCursors[ImGuiMouseCursor_Arrow]);
		glfwSetInputMode(window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void InterfaceController::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(mouseDown))
		mouseDown[button] = true;
}

void InterfaceController::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)xOffset;
	io.MouseWheel += (float)yOffset;
}

void InterfaceController::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	// Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void InterfaceController::CharCallback(GLFWwindow* window, unsigned int c)
{
	ImGuiIO& io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		io.AddInputCharacter((unsigned short)c);
}