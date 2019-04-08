#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "environment.h"

namespace libnes
{
	template <typename T>
	class Vector
	{
	private:
		T * data;
		uint32_t length;
		uint32_t capacity;

	public:
		Vector() : Vector(10)
		{

		}

		Vector(uint32_t capacity) : length(0), capacity(capacity)
		{
			data = new T[capacity];
		}

		T& operator[](uint32_t index)
		{
			assert(index < length);

			return data[index];
		}

		T& operator[](uint32_t index) const
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

		int32_t IndexOf(const T& element)
		{
			for (uint32_t i = 0; i < length; ++i)
			{
				if (data[i] == element)
					return i;
			}

			return -1;
		}

		bool Remove(const T& element)
		{
			int32_t index = IndexOf(element);

			if (index == -1)
				return false;

			RemoveIndex(index);
			return true;
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