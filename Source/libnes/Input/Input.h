#ifndef _INPUT_H_
#define _INPUT_H_

#include "environment.h"

namespace libnes
{
	class Controller;
	class DummyController;

	class Input
	{
	private:
		Controller* ports[2];

		DummyController* dummyController;

	public:
		Input();
		~Input();

		void BindController(uint8_t port, Controller* controller);

		uint8_t Read(uint16_t address);
		uint8_t Peek(uint16_t address) const;
		void Write(uint16_t address, uint8_t value);
	};

}

#endif