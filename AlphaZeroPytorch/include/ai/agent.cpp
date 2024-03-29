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

AlphaZero::ai::Agent::Agent(const char* device)
{
	this->tree = {};
	this->model = std::make_unique<Model>(device);
}

std::pair<int, std::pair<std::vector<int>, float>> AlphaZero::ai::Agent::getAction(std::shared_ptr<Game::GameState> state, bool proabilistic)
{
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(0);
#endif
	auto tree = this->getTree();
	tree->MCTSIter = 0;
	Node* node = tree->addNode(state);
#if threads > 0
	std::vector<std::thread> threadvec;
	for (int i = 0; i < threads; i++) {
		threadvec.push_back(std::thread(runSimulationsCaller, this, node));
	}
#endif
	runSimulationsCaller(this, node, tree);
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