#include "TxROM.h"

#include "CartridgeHeader.h"

using namespace libnes;

TxROM::TxROM(const CartridgeHeader_iNES& header, const uint8_t* buffer) : Mapper(header), registers(), usesChrRam(false), irqEnabled(false), irqReload(false)
{
	// Reset registers
	registers.bankSelect = 0x00;
	registers.mirroring = 0;
	registers.prgRamProtect = 0;
	registers.irqLatch = 0;
	registers.irqCounter = 0;

	for (uint8_t i = 0; i < 8; ++i)
		registers.banks[i] = 0;

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

TxROM::~TxROM()
{
	SAFE_DELETE_ARRAY(prgRom);
	SAFE_DELETE_ARRAY(prgRam);
	SAFE_DELETE_ARRAY(chrMem);
}

void TxROM::SelectBanks()
{
	// Determine PRG ROM bank indices
	uint8_t prgRomBankIndices[4];

	prgRomBankIndices[1] = (registers.banks[7] & 0x3F);
	prgRomBankIndices[3] = PrgBankCount() - 1;

	if (READ_BIT(registers.bankSelect, 6))
	{
		prgRomBankIndices[0] = PrgBankCount() - 2;
		prgRomBankIndices[2] = (registers.banks[6] & 0x3F);
	}
	else
	{
		prgRomBankIndices[0] = (registers.banks[6] & 0x3F);
		prgRomBankIndices[2] = PrgBankCount() - 2;
	}

	// Resolve PRG ROM pointers
	for (uint8_t bank = 0; bank < 4; ++bank)
		prgRomBanks[bank] = prgRom + (prgRomBankIndices[bank] << 13);

	// No RAM bank switching
	prgRamBank = prgRam;

	// Determine CHR bank indices
	uint8_t chrBankIndices[8];

	if (READ_BIT(registers.bankSelect, 7))
	{
		chrBankIndices[0] = registers.banks[2];
		chrBankIndices[1] = registers.banks[3];
		chrBankIndices[2] = registers.banks[4];
		chrBankIndices[3] = registers.banks[5];

		chrBankIndices[4] = (registers.banks[0] & 0xFE);
		chrBankIndices[5] = (registers.banks[0] | 0x01);

		chrBankIndices[6] = (registers.banks[1] & 0xFE);
		chrBankIndices[7] = (registers.banks[1] | 0x01);
	}
	else
	{
		chrBankIndices[0] = (registers.banks[0] & 0xFE);
		chrBankIndices[1] = (registers.banks[0] | 0x01);

		chrBankIndices[2] = (registers.banks[1] & 0xFE);
		chrBankIndices[3] = (registers.banks[1] | 0x01);

		chrBankIndices[4] = registers.banks[2];
		chrBankIndices[5] = registers.banks[3];
		chrBankIndices[6] = registers.banks[4];
		chrBankIndices[7] = registers.banks[5];
	}

	// Resolve CHR pointers
	for (uint8_t bank = 0; bank < 8; ++bank)
		chrBanks[bank] = chrMem + (chrBankIndices[bank] << 10);

}

uint8_t TxROM::PeekMain(uint16_t address) const
{
	// Not connected
	if (address < 0x6000)
		return 0xFF;

	// PRG RAM area
	if (address < 0x8000)
		return prgRamBank[address & 0x1FFF];

	// PRG ROM area
	uint8_t bank = (address >> 13) & 0x03;
	return prgRomBanks[bank][address & 0x1FFF];
}


void TxROM::WriteMain(uint16_t address, uint8_t value)
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

	// Control registers
	switch ((address >> 13) & 0x03)
	{
		// 0x8000 - 0x9FFF
		case 0x00:
			if (address & 0x01)
			{
				// Bank data
				registers.banks[registers.bankSelect & 0x07] = value;
				SelectBanks();
			}
			else
			{
				// Bank select
				registers.bankSelect = value;
			}

			break;

		// 0xA000 - 0xBFFF
		case 0x01:
			if (address & 0x01)
			{
				// PRGM RAM protect
				registers.prgRamProtect = value;
			}
			else
			{
				// Mirroring
				registers.mirroring = value;
			}
			break;

		// 0xC000 - 0xDFFF
		case 0x02:
			if (address & 0x01)
			{
				// IRQ reload
				irqReload = true;
			}
			else
			{
				// IRQ latch
				registers.irqLatch = value;
			}
			break;

		// 0xE000 - 0xFFFF
		case 0x03:
			// Odd = enable IRQ, even = disable IRQ
			irqEnabled = TEST_BIT(address, 0);
			break;

	}
}

uint8_t TxROM::PeekVideo(uint16_t address) const
{
	uint8_t bank = (address >> 10) & 0x07;
	return chrBanks[bank][address & 0x03FF];
}

void TxROM::WriteVideo(uint16_t address, uint8_t value)
{
	if (!usesChrRam)
		return;

	uint8_t bank = (address >> 10) & 0x07;
	chrBanks[bank][address & 0x03FF] = value;
}

NametableMirroring TxROM::GetMirroring() const
{
	if (header.DisableMirroring())
		return MIRROR_NONE;
	else
		return (registers.mirroring & 0x01) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL;
}

bool TxROM::CountScanline()
{
	if (irqReload)
	{
		registers.irqCounter = registers.irqLatch;
		irqReload = false;

		return false;
	}
	else if (registers.irqCounter == 0)
	{
		registers.irqCounter = registers.irqLatch;

		return irqEnabled;
	}
	else
	{
		--registers.irqCounter;

		return false;
	}
}