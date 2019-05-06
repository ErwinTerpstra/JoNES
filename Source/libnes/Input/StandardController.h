#ifndef _STANDARD_CONTROLLER_H_
#define _STANDARD_CONTROLLER_H_

#include "Controller.h"

namespace libnes
{
	class StandardController : public Controller
	{
	public:
		enum Button
		{
			BUTTON_A,
			BUTTON_B,
			BUTTON_SELECT,
			BUTTON_START,
			BUTTON_UP,
			BUTTON_DOWN,
			BUTTON_LEFT,
			BUTTON_RIGHT,

			BUTTON_COUNT
		};
	private:
		bool buttons[BUTTON_COUNT];

		uint8_t buttonIdx;

	public:
		StandardController()
		{
			for (uint32_t i = 0; i < BUTTON_COUNT; ++i)
				buttons[i] = false;
		}

		void SetButtonState(Button button, bool state)
		{
			buttons[button] = state;
		}

		void SetStrobe(bool strobe)
		{
			Controller::SetStrobe(strobe);

			if (strobe)
				buttonIdx = 0;
		}

		uint8_t Read()
		{
			if (strobe)
				buttonIdx = 0;

			if (buttonIdx < BUTTON_COUNT)
				return buttons[buttonIdx++] ? 1 : 0;
			else
				return 1;
		}

		uint8_t Peek() const
		{
			if (buttonIdx < BUTTON_COUNT)
				return buttons[buttonIdx] ? 1 : 0;
			else
				return 1;
		}

	};
}

#endif