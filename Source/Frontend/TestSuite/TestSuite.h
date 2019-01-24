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
		void Start();

	};
}

#endif