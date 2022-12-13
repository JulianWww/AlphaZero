#include <iostream>
#include <io.hpp>
#include <config.hpp>

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
#if LossLogger
	debug::log::lossLogger loss("logs/games/loss.bin");
	std::ofstream out("logs/games/loss.json");
	loss.render(out);
	out.close();
#endif
	return 1;
}