#include "playGame.hpp"


void AlphaZero::test::playGame(std::shared_ptr<Game::Game> game, std::shared_ptr<ai::Agent> player1, std::shared_ptr<ai::Agent> player2, int goesFirst)
{
	if (goesFirst == 0) {
		goesFirst  = 1;
		if (rand() % 2) {
			goesFirst = -1;
		}
	}
	player1->tree->reset();
	player2->tree->reset();
	std::unordered_map<int, std::shared_ptr<ai::Agent>> players = { {goesFirst, player1},{-goesFirst, player2} };
	int action;
	while (!game->state->done) {
		action = players[game->state->player]->getAction(game, false).first;
		game->takeAction(action);
	}
}

void AlphaZero::ai::train(int version)
{
	unsigned short iteration = 0;
	std::shared_ptr<Memory> memory = std::make_shared<Memory>();
	std::shared_ptr<Game::Game> game = std::make_shared<Game::Game>();
	std::shared_ptr<Agent> currentAgent = std::make_shared<Agent>(game, version);
	std::shared_ptr<Agent> bestAgent = std::make_shared<Agent>(game, version);
#if loadVersion >= 0
	bestAgent->model->load(loadVersion);
	currentAgent->model->load(loadVersion);
#endif
	// TODO bestAgent->model->save(0);
	if (true) { // TODO revert to while !!!
		memory->active = true;
#if MainLogger
		debug::log::mainLogger->info("playing generational Games");
#endif
		std::cout << "hello this is working -------------------------------------------------------------------------" << std::endl;
		playGames(game, bestAgent, bestAgent, memory, probabilitic_moves, EPOCHS);
		std::cout << "memory size is: " << memory->memory.size() << std::endl;
		if (memory->memory.size() > memory_size) {
			std::cout << "training" << std::endl;
#if ProfileLogger
			debug::Profiler::profiler.switchOperation(5);
			debug::log::profileLogger->info("performing Training of model");
			debug::log::profileLogger->info("----------------------------------------------------------------------------------");
#endif
			currentAgent->fit(memory, iteration);
#if ProfileLogger
			debug::Profiler::profiler.stop();
			debug::Profiler::profiler.log();
#endif
			memory->active = false;
			auto score = playGames(game, bestAgent, currentAgent, memory, Turnement_probabiliticMoves, TurneyEpochs);

			if (score[currentAgent] > score[bestAgent] * scoringThreshold) {
				version++;
				/*TODO bestAgent->model->setWeights(currentAgent->model->getWeights());
				bestAgent->model->save(version);*/
				std::printf("new version saved");
			}
			else {
				std::printf("retaining ___________________________________________________________________________________________");
			}
		}
		iteration++;
	}
}

std::unordered_map<std::shared_ptr<AlphaZero::ai::Agent>, int> AlphaZero::ai::playGames(std::shared_ptr<Game::Game> game, std::shared_ptr<Agent> agent1, std::shared_ptr<Agent> agent2, std::shared_ptr<Memory> memory, int probMoves, int Epochs, int goesFist)
{
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(3);
#endif
	std::unordered_map<std::shared_ptr<Agent>, int> scores = {
		{agent1, 0},
		{agent2, 0}
	};
	std::cout << "playing game ";
	for (int epoch = 0; epoch < Epochs; epoch++) {
		std::cout << epoch + 1 << " ";
#if ProfileLogger
		debug::Profiler::profiler.switchOperation(3);
#endif
#if MainLogger
		std::cout << "playing game: " << epoch << std::endl;
		debug::log::mainLogger->info("================================================================");
		debug::log::mainLogger->info("====================== playing Next match ======================");
		debug::log::mainLogger->info("================================================================");
#endif
		if (goesFist == 0) {
			goesFist = rand() % 2 * 2 - 1;
		}
		std::unordered_map<int, std::shared_ptr<Agent>> players = {
			{goesFist, agent1},
			{-goesFist,agent2}
		};
		agent1->tree->reset();
		agent2->tree->reset();
#if MainLogger
		debug::log::mainLogger->info("player {} will start", goesFist);
#endif
		game->reset();
		int turn = 0;
		while (!game->state->done) {
			turn++;
			auto actionData = players[game->state->player]->getAction(game, probMoves > turn);
			memory->commit(game->state, actionData.second);
#if MainLogger
			game->state->render(debug::log::mainLogger);
			debug::log::logVector(debug::log::mainLogger, actionData.second);
			debug::log::mainLogger->info("selected action is: {}", actionData.first);
#endif
			game->takeAction(actionData.first);
		}
#if MainLogger
		game->state->render(debug::log::mainLogger);
#endif
#if ProfileLogger
		debug::Profiler::profiler.switchOperation(4);
#endif
		memory->updateMemory(game->state->player, std::get<0>(game->state->val));
		scores[players[game->state->player * std::get<0>(game->state->val)]] += 1;
#if ProfileLogger
		debug::Profiler::profiler.stop();
		debug::Profiler::profiler.log();
#endif
	}
	std::cout << std::endl;
	return scores;
}
