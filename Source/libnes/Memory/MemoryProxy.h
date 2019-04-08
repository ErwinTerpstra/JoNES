#ifndef _MEMORY_PROXY_H_
#define _MEMORY_PROXY_H_

#include "environment.h"

#include "MemoryInterface.h"
#include "MemoryBus.h"

namespace libnes
{
	class MemoryProxy : public MemoryInterface
	{
	private:
		MemoryBus* bus;
		MemoryInterface* original;

	public:

		MemoryProxy(MemoryBus* bus) : bus(bus), original(NULL)
		{
			Bind();
		}

		~MemoryProxy()
		{
			Unbind();
		}

		void Bind()
		{
			if (original == NULL)
			{
				original = bus->GetInterface();
				bus->BindInterface(this);
			}
		}

		void Unbind()
		{
			if (original != NULL)
			{
				bus->BindInterface(original);
				original = NULL;
			}
		}

		uint8_t Read(uint16_t address)
		{
			return original->Read(address);
		}

		uint8_t Peek(uint16_t address) const
		{
			return original->Peek(address);
		}

		void Write(uint16_t address, uint8_t value)
		{
			original->Write(address, value);
		}

	};

}

#endif