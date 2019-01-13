#ifndef _DEBUG_H_
#define _DEBUG_H_

namespace jones
{

	class Debug
	{

	private:

		
	public:
		
		static void Print(const char* msg, va_list args)
		{
			vprintf(msg, args);
		}

		static void Print(const char* msg, ...)
		{
			va_list args;
			va_start(args, msg);

			vprintf(msg, args);
			va_end(args);
		}

		static bool AssertHandler(const char* code, const char* file, const uint32_t line)
		{
			Print("Assert failed!\n%s at %s:%d\n", code, file, line);
			return true;
		}

		static bool Halt()
		{
			__debugbreak();
			return true;
		}

		static void Break()
		{
			__debugbreak();
		}
	};

#ifdef _DEBUG
#define assert(x) ((void)(!(x) && Debug::AssertHandler(#x, __FILE__, __LINE__) && Debug::Halt()))
#else
#define assert(x)
#endif

}

#endif