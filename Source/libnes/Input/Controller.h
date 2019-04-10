#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "environment.h"

namespace libnes
{

	class Controller
	{
	protected:
		bool strobe;

	public:
		Controller() : strobe(false)
		{

		}

		virtual void SetStrobe(bool strobe)
		{
			this->strobe = strobe;
		}

		virtual uint8_t Read() = 0;
		virtual uint8_t Peek() const = 0;
	};
}

#endif