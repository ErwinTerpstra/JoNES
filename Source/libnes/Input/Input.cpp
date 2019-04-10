#include "Input.h"

#include "Controller.h"
#include "DummyController.h"

#include "nes.h"
#include "util.h"

using namespace libnes;

Input::Input()
{
	dummyController = new DummyController();

	ports[0] = dummyController;
	ports[1] = dummyController;
}

Input::~Input()
{
	SAFE_DELETE(dummyController);
}

void Input::BindController(uint8_t port, Controller* controller)
{
	ports[port] = controller != NULL ? controller : dummyController;
}

uint8_t Input::Read(uint16_t address)
{
	uint8_t port = address - NES_CPU_REG_JOY0;
	return ((address >> 8) & 0xE0) | ports[port]->Read();
}

uint8_t Input::Peek(uint16_t address) const
{
	uint8_t port = address - NES_CPU_REG_JOY0;
	return ((address >> 8) & 0xE0) | ports[port]->Peek();
}

void Input::Write(uint16_t address, uint8_t value)
{
	if (address == NES_CPU_REG_JOY0)
	{
		bool strobe = TEST_BIT(value, 0);
		ports[0]->SetStrobe(strobe);
		ports[1]->SetStrobe(strobe);
	}
}