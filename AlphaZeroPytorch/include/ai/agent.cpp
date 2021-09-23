#include "agent.hpp"
#include <stdlib.h>

#if not Training
std::pair<int, std::vector<float>> AlphaZero::ai::User::getAction(std::shared_ptr<Game::Game> game, bool proabilistic)
{
	std::system("cls");
	game->render();
	int action = -1;
	while (std::find(game->state->allowedActions.begin(), game->state->allowedActions.end(), action) == game->state->allowedActions.end()) {// while(not game->state->allowedActions.contains(action))
		std::cout << "your Action: ";
		std::cin >> action;
	}
	return { action, std::vector<float>() };
}
#endif

AlphaZero::ai::Agent::Agent(std::shared_ptr<Game::Game> game, int version) // TODO remove int version from this function
{
	this->model = std::make_shared<AlphaZero::ai::Model>();
	this->tree = std::make_shared<MCTS>(game->state);
}

std::pair<int, std::vector<float>> AlphaZero::ai::Agent::getAction(std::shared_ptr<Game::Game> game, bool proabilistic)
{
#if threads > 0
	std::vector<std::thread> threadvec;
	for (int i = 0; i < threads; i++) {
		threadvec.push_back(std::thread(runSimulationsCaller, this, game->state));
	}
#endif
	runSimulationsCaller(this, game->state);
#if threads > 0
	for (auto& thread : threadvec) {
		thread.join();
	}
#endif
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(3);
#endif
	std::pair<int, std::vector<float>> actionVals;
	if (proabilistic) {
		actionVals = this->prabilisticAction(this->tree->getNode(game->state->id()));
	}
	else {
		 actionVals = this->derministicAction(this->tree->getNode(game->state->id()));
	}
	return actionVals;
}
