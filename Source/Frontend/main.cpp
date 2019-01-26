#include "stdafx.h"

#include "App.h"
#include "TestSuite/TestSuite.h"

int main()
{
	JoNES::App app;

	if (!app.Init())
		return -1;
	
	const char* romFileName = "../../Roms/tests/nestest.nes";
	if (!app.LoadROM(romFileName))
		printf("[JoNES]: Failed to load ROM file %s\n", romFileName);

	JoNES::TestSuite testSuite(app.emulator);
	testSuite.RunAutomated("../../Logs/JoNES_nestest.log");

	while (!app.ShouldExit())
	{
		app.Update();
		app.Render();
	}

	app.Shutdown();

	return 0;
}

