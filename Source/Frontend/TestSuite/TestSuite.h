#ifndef _TEST_SUITE_H_
#define _TEST_SUITE_H_

namespace libnes
{
	class Emulator;
}

namespace JoNES
{
	
	class TestSuite
	{
	private:
		libnes::Emulator* emulator;

		const char* logFileName;

	public:
		TestSuite(libnes::Emulator* emulator, const char* logFileName);
		
		void Start();
		void ExecuteNextInstruction();

	private:
		void WriteCurrentStateToLog();

	};
}

#endif