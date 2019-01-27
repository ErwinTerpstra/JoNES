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

	public:
		TestSuite(libnes::Emulator* emulator);
		~TestSuite();
		
		void RunAutomated(const char* logFileName);

	private:
		uint32_t WriteCurrentStateToLog(char* buffer, uint32_t bufferSize);

	};
}

#endif