#ifndef _MAPPER_H_
#define _MAPPER_H_

#include "environment.h"
#include "CartridgeHeader.h"

namespace libnes
{
	class Mapper
	{
	protected:
		const CartridgeHeader_iNES& header;

	public:
		Mapper(const CartridgeHeader_iNES& header) : header(header)
		{

		}

		virtual ~Mapper() { }

		virtual uint8_t ReadMain(uint16_t address)
		{
			return PeekMain(address);
		}

		virtual uint8_t ReadVideo(uint16_t address)
		{
			return PeekVideo(address);
		};

		virtual uint8_t PeekMain(uint16_t address) const = 0;
		virtual void WriteMain(uint16_t address, uint8_t value) = 0;

		virtual uint8_t PeekVideo(uint16_t address) const = 0;
		virtual void WriteVideo(uint16_t address, uint8_t value) = 0;

		virtual bool GetInternalVideoRamA10(uint16_t address) const
		{
			switch (GetMirroring())
			{
			default:
			case MIRROR_ONE_BANK_LOWER:
				return false;

			case MIRROR_ONE_BANK_UPPER:
				return true;

			case MIRROR_VERTICAL:
				return TEST_BIT(address, 10);

			case MIRROR_HORIZONTAL:
				return TEST_BIT(address, 11);
			}
		}

		virtual bool GetInternalVideoRamEnabled(uint16_t address) const
		{
			return TEST_BIT(address, 13);
		};

		virtual NametableMirroring GetMirroring() const
		{
			return header.GetMirroring();
		}
	};
}

#endif