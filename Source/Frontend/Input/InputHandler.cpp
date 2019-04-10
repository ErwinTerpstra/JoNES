#include "stdafx.h"

#include "libnes/libnes.h"

#include "InputHandler.h"

#include "Window.h"

#include "imgui/imgui.h"

using namespace libnes;
using namespace JoNES;

InputHandler::InputHandler(Device* device, Window* window) : device(device), window(window)
{
	p1 = new StandardController();

	device->input->BindController(0, p1);
}

InputHandler::~InputHandler()
{
	SAFE_DELETE(p1);
}

void InputHandler::Update()
{
	GLFWwindow* handle = window->GetHandle();

	ImGuiIO& io = ImGui::GetIO();

	p1->SetButtonState(StandardController::BUTTON_UP,		io.KeysDown[io.KeyMap[ImGuiKey_UpArrow]]);
	p1->SetButtonState(StandardController::BUTTON_DOWN,		io.KeysDown[io.KeyMap[ImGuiKey_DownArrow]]);
	p1->SetButtonState(StandardController::BUTTON_LEFT,		io.KeysDown[io.KeyMap[ImGuiKey_LeftArrow]]);
	p1->SetButtonState(StandardController::BUTTON_RIGHT,	io.KeysDown[io.KeyMap[ImGuiKey_RightArrow]]);

	p1->SetButtonState(StandardController::BUTTON_SELECT,	io.KeysDown[io.KeyMap[ImGuiKey_Backspace]]);
	p1->SetButtonState(StandardController::BUTTON_START,	io.KeysDown[io.KeyMap[ImGuiKey_Enter]]);

	p1->SetButtonState(StandardController::BUTTON_A,		io.KeysDown[io.KeyMap[ImGuiKey_Z]]);
	p1->SetButtonState(StandardController::BUTTON_B,		io.KeysDown[io.KeyMap[ImGuiKey_X]]);
}