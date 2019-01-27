#ifndef _LIBNES_DEBUG_H_
#define _LIBNES_DEBUG_H_

#include "environment.h"
#include <stdio.h>

namespace libnes
{
	namespace debug
	{
		static void Halt()
		{
			__debugbreak();
		}
	}
}

#endif