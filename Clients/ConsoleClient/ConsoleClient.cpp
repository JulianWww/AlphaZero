// ConsoleClient.cpp : Defines the entry point for the application.
//

#include "ConsoleClient.h"
#include <agent.hpp>

using namespace std;

void playGame(std::shared_ptr<Agents::Agent>agent1, std::shared_ptr<Agents::Agent>agent2, std::shared_ptr<AlphaZero::Game::Game>game)
{
	while (!game->state->done)
	{
		int action;
		switch (game->state->player)
		{
		case(1):{
			action = agent1->getAction(game);
			break;
		}
		case(-1): {
			action = agent2->getAction(game);
			break;
		}
		}
		game->takeAction(action);
	}
}

int main()
{
	auto game = std::make_shared<AlphaZero::Game::Game>();
	auto user = std::make_shared<Agents::User>();
	auto AI = std::make_shared<Agents::RemoteAgent>("35.240.231.50", 12345);
	playGame(user, AI, game);
	return 0;
}
