#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "environment.h"

namespace libnes
{
	template <typename T>
	class Array
	{
	private:
		T * data;
		uint32_t count;
		uint32_t size;

	public:
		Array(uint32_t size) : count(0), size(size)
		{
			data = new T[size];
		}
	};
}

#endif