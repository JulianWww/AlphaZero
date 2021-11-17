#include <test/testSuit.hpp>



int main(int argc, char** argv)
{
	AlphaZero::test::runTests();
	//AlphaZero::Game::test();
	
	return 1;
}

/*	std::vector<int> av;
	for (size_t idx = 0; idx < 64; idx++)
	{
		av.push_back(idx);
	}
	auto ids = AlphaZero::Game::identities(game.state, av);
	std::vector<AlphaZero::Game::GameState*> states(2);

	for (size_t index = 0; index < 4; index++)
	{
		states[0] = ids[2*index+0].first.get();
		states[1] = ids[2*index+1].first.get();
		AlphaZero::Game::renderStates(states);
	}*/