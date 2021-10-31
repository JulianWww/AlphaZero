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

AlphaZero::ai::Agent::Agent(std::vector<char*> devices)
{
	this->model = std::make_shared<AlphaZero::ai::modelManager>();
	for (auto& device : devices)
	{
		new modelWorker(device, this->model.get());
	}
	this->tree = std::make_shared<AlphaZero::ai::MCTS>();
}

std::pair<int, std::vector<int>> AlphaZero::ai::Agent::getAction(std::shared_ptr<Game::GameState> state, bool proabilistic)
{
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(0);
#endif
	this->tree->MCTSIter = 0;
	Node* node = this->tree->addNode(state);
#if threads > 0
	std::vector<std::thread> threadvec;
	for (int i = 0; i < threads; i++) {
		threadvec.push_back(std::thread(runSimulationsCaller, this, node));
	}
#endif
	runSimulationsCaller(this, node);
#if threads > 0
	for (auto& thread : threadvec) {
		thread.join();
	}
#endif
	try {
#if ProfileLogger
		debug::Profiler::profiler.switchOperation(3);
#endif
		if (proabilistic) {
			return this->prabilisticAction(node);
		}
		else {
			return this->derministicAction(node);
		}
	}
	catch (const std::exception& ex) {
		std::cerr << "\33[31;1mError in Agent::getAction\33[0m" << std::endl;
		std::cerr << ex.what() << std::endl;
		throw ex;
	}
}