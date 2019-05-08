#ifndef _MAPPER_H_
#define _MAPPER_H_

#include "environment.h"

namespace libnes
{
	struct CartridgeHeader_iNES;

	class Mapper
	{
	protected:
		const CartridgeHeader_iNES& header;

	public:
		Mapper(const CartridgeHeader_iNES& header) : header(header)
		{

		}

		virtual ~Mapper() { }

		virtual uint8_t ReadMain(uint16_t address) = 0;
		virtual uint8_t PeekMain(uint16_t address) const = 0;
		virtual void WriteMain(uint16_t address, uint8_t value) = 0;

		virtual uint8_t ReadVideo(uint16_t address) = 0;
		virtual uint8_t PeekVideo(uint16_t address) const = 0;
		virtual void WriteVideo(uint16_t address, uint8_t value) = 0;

		virtual bool GetInternalVideoRamA10(uint16_t address) const = 0;
		virtual bool GetInternalVideoRamEnabled(uint16_t address) const = 0;
	};
}

#endif