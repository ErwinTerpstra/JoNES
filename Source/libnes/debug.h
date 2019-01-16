#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "environment.h"
#include <stdio.h>

#ifdef _DEBUG

namespace libnes
{
	namespace debug
	{
		static void Halt()
		{
			__debugbreak();
		}

		static bool AssertHandler(const char* code, const char* file, const uint32_t line)
		{
			printf("Assert failed!\n%s at %s:%d\n", code, file, line);
			Halt();

			return true;
		}
	}
}

#define assert(x) ((void)(!(x) && libnes::debug::AssertHandler(#x, __FILE__, __LINE__)))
#else
#define assert(x)
#endif

#endif