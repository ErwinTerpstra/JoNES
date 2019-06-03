#include "SxROM.h"

#include "CartridgeHeader.h"

using namespace libnes;

SxROM::SxROM(const CartridgeHeader_iNES& header, const uint8_t* buffer) : Mapper(header), registers(), usesChrRam(false)
{
	// Reset registers
	registers.load = 0x00;
	registers.loadWriteCount = 0;

	registers.control = 0x0C;
	registers.chrBank0 = 0x00;
	registers.chrBank1 = 0x00;
	registers.prgBank = 0x00;

	// Setup PRG ROM
	prgRomSize = header.GetPrgRomSizeInBytes();

	prgRom = new uint8_t[prgRomSize];
	const uint8_t* prgRomSrc = buffer + sizeof(CartridgeHeader_iNES);
	memcpy(prgRom, prgRomSrc, prgRomSize);

	// Set PRG RAM
	prgRamSize = header.GetPrgRamSizeInBytes();

	prgRam = new uint8_t[prgRamSize];
	memset(prgRam, 0x00, prgRamSize);

	// Setup CHR ROM or RAM
	uint32_t chrRomSize = header.GetChrRomSizeInBytes();
	if (chrRomSize > 0)
	{
		chrMem = new uint8_t[chrRomSize];

		const uint8_t* chrRomSrc = prgRomSrc + prgRomSize;
		memcpy(chrMem, chrRomSrc, chrRomSize);

		usesChrRam = false;
	}
	else
	{
		uint32_t chrRamSize = 0x2000;

		chrMem = new uint8_t[chrRamSize];
		memset(chrMem, 0x00, chrRamSize);
		
		usesChrRam = true;
	}

	SelectBanks();
}

SxROM::~SxROM()
{
	SAFE_DELETE_ARRAY(prgRom);
	SAFE_DELETE_ARRAY(prgRam);
	SAFE_DELETE_ARRAY(chrMem);
}

void SxROM::SelectBanks()
{
	// Determine PRG ROM bank indices
	uint8_t prgRomBankIndices[2];
	switch ((registers.control >> 2) & 0x03)
	{
		case 0x00:
		case 0x01:
		{
			// 32KB bank mode, ignore the lowest bit of the bank number
			prgRomBankIndices[0] = registers.prgBank & 0x0E;
			prgRomBankIndices[1] = prgRomBankIndices[0] + 1;
			break;
		}

		case 0x02:
		{
			prgRomBankIndices[0] = 0;							// 0x8000 is fixed to first bank
			prgRomBankIndices[1] = registers.prgBank & 0x0F;	// 0xC000 is a switchable bank
			break;
		}

		case 0x03:
		{
			prgRomBankIndices[0] = registers.prgBank & 0x0F;	// 0x8000 is a switchable bank
			prgRomBankIndices[1] = (prgRomSize >> 14) - 1;		// 0xC000 is fixed to last bank
			break;
		}
	}

	// Resolve PRG ROM pointers
	prgRomBanks[0] = prgRom + (prgRomBankIndices[0] << 14);
	prgRomBanks[1] = prgRom + (prgRomBankIndices[1] << 14);

	// No RAM bank switching yet...
	prgRamBank = prgRam;

	// Determine CHR bank indices
	uint8_t chrBankIndices[2];

	// Check if we use 4KB or 8KB mode
	if (TEST_BIT(registers.control, 4))
	{
		chrBankIndices[0] = registers.chrBank0 & 0x0F;
		chrBankIndices[1] = registers.chrBank1 & 0x0F;
	}
	else
	{
		// 8KB mode ignores the lowest bit of the bank register
		chrBankIndices[0] = registers.chrBank0 & 0x0E;
		chrBankIndices[1] = chrBankIndices[0] + 1;
	}

	// Resolve CHR pointers
	chrBanks[0] = chrMem + (chrBankIndices[0] << 12);
	chrBanks[1] = chrMem + (chrBankIndices[1] << 12);

}

uint8_t SxROM::PeekMain(uint16_t address) const
{
	// Not connected
	if (address < 0x6000)
		return 0xFF;

	// PRG RAM area
	if (address < 0x8000)
		return prgRamBank[address & 0x1FFF];

	uint8_t bank = READ_BIT(address, 14);
	return prgRomBanks[bank][address & 0x3FFF];
}


void SxROM::WriteMain(uint16_t address, uint8_t value)
{
	// Not connected
	if (address < 0x6000)
		return;

	// PRG RAM area
	if (address < 0x8000)
	{
		prgRamBank[address & 0x1FFF] = value;
		return;
	}

	if (TEST_BIT(value, 7))
	{
		registers.load = 0;
		registers.loadWriteCount = 0;
		registers.control |= 0x0C;
	}
	else
	{
		registers.load = (registers.load >> 1) | ((value & 0x01) << 4);
		++registers.loadWriteCount;

		if (registers.loadWriteCount >= 5)
		{
			// Bit 13 & 14 control which register to write to
			switch ((address >> 13) & 0x03)
			{
			case 0x00:
				registers.control = registers.load;
				break;

			case 0x01:
				registers.chrBank0 = registers.load;
				break;

			case 0x02:
				registers.chrBank1 = registers.load;
				break;

			case 0x03:
				registers.prgBank = registers.load;
				break;
			}

			registers.load = 0;
			registers.loadWriteCount = 0;

			SelectBanks();
		}
	}
}

uint8_t SxROM::PeekVideo(uint16_t address) const
{
	uint8_t bank = READ_BIT(address, 12);
	return chrBanks[bank][address & 0x0FFF];
}

void SxROM::WriteVideo(uint16_t address, uint8_t value)
{
	if (!usesChrRam)
		return;

	uint8_t bank = READ_BIT(address, 12);
	chrBanks[bank][address & 0x0FFF] = value;
}

NametableMirroring SxROM::GetMirroring() const
{
	switch (registers.control & 0x03)
	{
	default:
	case 0x00:
		return MIRROR_ONE_BANK_LOWER;

	case 0x01:
		return MIRROR_ONE_BANK_UPPER;

	case 0x02:
		return MIRROR_VERTICAL;

	case 0x03:
		return MIRROR_HORIZONTAL;
	}
}