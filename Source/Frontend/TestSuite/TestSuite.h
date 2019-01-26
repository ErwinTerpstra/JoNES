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

		char* lineBuffer;

	public:
		TestSuite(libnes::Emulator* emulator);
		~TestSuite();
		
		void RunAutomated(const char* logFileName);

	private:
		void WriteCurrentStateToLog(const char* logFileName);

	};
}

#endif