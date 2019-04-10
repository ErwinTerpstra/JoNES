#ifndef _DUMMY_CONTROLLER_H_
#define _DUMMY_CONTROLLER_H_

#include "Controller.h"

namespace libnes
{
	class DummyController : public Controller
	{

	public:
		uint8_t Read()
		{
			return 0;
		}

		uint8_t Peek() const
		{
			return 0;
		}
	};
}

#endif