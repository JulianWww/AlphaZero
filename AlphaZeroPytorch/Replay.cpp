#include <iostream>
#include <io.hpp>
#include <config.hpp>

// sriet replay to console from file (not realy used)

int main(int argc, char ** argv)
{
#if SaverType == 2
	auto saver = AlphaZero::io::ActionsOnly::GameSaver();
#elif SaverType == 1
	auto saver = AlphaZero::io::FullState::GameSaver();
#endif
#if SaverType != 0
	saver.load("test.bin");
	saver.ConsoleReplay(0);
#endif
	return 1;
}