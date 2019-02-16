#ifndef _EVENT_H_
#define _EVENT_H_

#include "Vector.h"

namespace libnes
{
	class EventHandler
	{
	public:

		virtual void operator()() = 0;
	};

	template <class T>
	class EventHandlerProxy : public EventHandler
	{
	public:
		typedef void (T::*FunctionType)();

	private:
		T* object;
		FunctionType function;

	public:
		EventHandlerProxy(T* object, FunctionType function) : object(object), function(function)
		{

		}

		void operator()()
		{
			(object->*function)();
		}
	};

	class Event
	{
	private:
		Vector<EventHandler*> handlers;

	public:
		Event() : handlers()
		{

		}

		void RegisterEventHandler(EventHandler* handler)
		{
			handlers.Add(handler);
		}

		void UnregisterEventHandler(EventHandler* handler)
		{
			handlers.Remove(handler);
		}

		void Fire()
		{
			for (uint32_t i = 0; i < handlers.Length(); ++i)
				(*handlers[i])();
		}
	};
}

#endif