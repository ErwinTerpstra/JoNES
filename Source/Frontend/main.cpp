#include "stdafx.h"

#include "App.h"

int main()
{
	JoNES::App app;

	if (!app.Init())
		return -1;
	
	//const char* romFileName = "../../Roms/tests/nestest.nes";
	const char* romFileName = "../../Roms/Donkey Kong.nes";
	//const char* romFileName = "../../Roms/Zelda.nes";
	//const char* romFileName = "../../Roms/Super Mario Bros (World).nes";
	//const char* romFileName = "../../Roms/Super Mario Bros 3.nes";
	//const char* romFileName = "../../Roms/tests/instr_test-v5/official_only.nes";
	//const char* romFileName = "../../Roms/tests/instr_test-v5/rom_singles/01-basics.nes";

	if (!app.LoadROM(romFileName))
		printf("[JoNES]: Failed to load ROM file %s\n", romFileName);

	while (!app.ShouldExit())
		app.Update();

	app.Shutdown();

	return 0;
}

