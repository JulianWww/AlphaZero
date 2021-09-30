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

	currentAgent->identity = 0;
	bestAgent->identity = 1;
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
#if RenderGenAndTurneyProgress
		std::cout << "Playing Generational Games" << std::endl;
#endif

		playGames(game, bestAgent, bestAgent, memory, probabilitic_moves, EPOCHS);
		std::cout << "memory size is: " << memory->memory.size() << std::endl;
		if (memory->memory.size() > memory_size) {
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

#if RenderGenAndTurneyProgress
			std::cout << "Playing Turney" << std::endl;
#endif
			auto score = playGames(game, bestAgent, currentAgent, memory, Turnement_probabiliticMoves, TurneyEpochs);

			if (true){//(score[currentAgent->identity] > score[bestAgent->identity] * scoringThreshold) {
				version++;
				//TODO copy model weightsk
				currentAgent->model->save_as_current();
				bestAgent->model->load_current();
				bestAgent->model->save_version(version);
				std::printf("new version saved");
			}
			else {
				std::printf("retaining ___________________________________________________________________________________________");
			}
		}
		iteration++;
	}
}

std::unordered_map<int, int> AlphaZero::ai::playGames(std::shared_ptr<Game::Game> game, std::shared_ptr<Agent> agent1, std::shared_ptr<Agent> agent2, std::shared_ptr<Memory> memory, int probMoves, int Epochs, int goesFist)
{
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(3);
#endif
	std::unordered_map<int, int> scores;
	if (!agent1->identity == agent2->identity) {
		scores.insert({ agent1->identity, 0 });
		scores.insert({ agent2->identity, 0 });
	}
	for (int epoch = 0; epoch < Epochs; epoch++) {
#if RenderGenAndTurneyProgress
		jce::consoleUtils::render_progress_bar((float)epoch / (float)Epochs);
#endif
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

		if (!memory->active) {
			std::cout << "successfully reset" << std::endl;
		}
#if MainLogger
		debug::log::mainLogger->info("player {} will start", goesFist);
#endif
		game->reset();
		int turn = 0;
		if (!memory->active) {
			std::cout << "successfully reset" << std::endl;
		}
		while (!game->state->done) {
			turn++;
			if (!memory->active) {
				std::cout << "started turn: " << turn << std::endl;
			}
			auto actionData = players[game->state->player]->getAction(game, probMoves > turn);
			if (!memory->active) {
				std::cout << "got data" << std::endl;
			}
			memory->commit(game->state, actionData.second);
#if MainLogger
			game->state->render(debug::log::mainLogger);
			debug::log::logVector(debug::log::mainLogger, actionData.second);
			debug::log::mainLogger->info("selected action is: {}", actionData.first);
#endif
			game->takeAction(actionData.first);
			if (!memory->active) {
				std::cout << "took action: " << turn << std::endl;
			}
			game->render();
		}
#if MainLogger
		game->state->render(debug::log::mainLogger);
#endif
#if ProfileLogger
		debug::Profiler::profiler.switchOperation(4);
#endif
		if (!memory->active) {
			std::cout << "update memory" << std::endl;
		}
		memory->updateMemory(game->state->player, std::get<0>(game->state->val));
		if (!memory->active) {
			std::cout << "updated Memory" << std::endl;
		}
		if (!agent1->identity == agent2->identity) {
			scores[players[game->state->player * std::get<0>(game->state->val)]->identity] += 1;
		}
		if (!memory->active) {
		}
#if ProfileLogger
		debug::Profiler::profiler.stop();
		debug::Profiler::profiler.log();
#endif
	}

#if RenderGenAndTurneyProgress
	jce::consoleUtils::render_progress_bar(1.0f, true);
#endif
	return scores;
}

// https://github.com/JulianWww/AlphaZero/
