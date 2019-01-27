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
		uint32_t length;
		uint32_t capacity;

	public:
		Array(uint32_t capacity) : length(0), capacity(capacity)
		{
			data = new T[capacity];
		}

		T& operator[](uint32_t index)
		{
			assert(index < length);

			return data[index];
		}

		void Add(const T& element)
		{
			assert(length < capacity);

			data[length] = element;
			++length;
		}

		void RemoveIndex(uint32_t index)
		{
			assert(index < length);

			for (uint32_t i = index; i < length - 1; ++i)
				data[i] = data[i + 1];

			--length;
		}

		uint32_t Length() const
		{
			return length;
		}

		uint32_t Capacity() const
		{
			return capacity;
		}
	};
}

#endif