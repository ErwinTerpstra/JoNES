#ifndef _EMULATOR_H_
#define _EMULATOR_H_

namespace libnes
{
	struct NES;
	class Cartridge;

	class Emulator
	{
	private:
		NES* nes;

	public:
		Emulator();
		~Emulator();

		void InsertCartridge(Cartridge* cartridge);

		void Update(float time);
	};
}

#endif