#ifndef _FRONTEND_H_
#define _FRONTEND_H_

namespace jones
{
	class Frontend
	{
	public:
		static bool Init();
		static void Shutdown();

		static void ProcessEvents();
	};
}

#endif