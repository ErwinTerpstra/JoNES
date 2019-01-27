#ifndef _JONES_DEBUG_H_
#define _JONES_DEBUG_H_

namespace JoNES
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
	
}

#endif