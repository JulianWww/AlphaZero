#include "playGame.hpp"
#include <io.hpp>


void AlphaZero::test::playGame(std::shared_ptr<Game::Game> game, std::shared_ptr<ai::Agent> player1, std::shared_ptr<ai::Agent> player2, int goesFirst)
{
	if (goesFirst == 0) {
		goesFirst  = 1;
		if (rand() % 2) {
			goesFirst = -1;
		}
	}
	player1->reset();
	player2->reset();
	std::unordered_map<int, std::shared_ptr<ai::Agent>> players = { {goesFirst, player1},{-goesFirst, player2} };
	int action;
	while (!game->state->done) {
		action = players[game->state->player]->getAction(game->state, false).first;
		game->takeAction(action);
	}
}

void AlphaZero::ai::train(int version)
{
	unsigned short iteration = 0;
	std::shared_ptr<Memory> memory = std::make_shared<Memory>();
	std::shared_ptr<Game::Game> game = std::make_shared<Game::Game>();
	std::shared_ptr<Agent> currentAgent = std::make_shared<Agent>();
	std::shared_ptr<Agent> bestAgent = std::make_shared<Agent>();

	memory->load();
	char nameBuff[100];

	currentAgent->identity = 0;
	bestAgent->identity = 1;
#if loadVersion >= 0
	bestAgent->model->load(loadVersion);
	currentAgent->model->load(loadVersion);
#else
	currentAgent->model->copyModel(bestAgent->model);
#endif

	// TODO bestAgent->model->save(0);
	while (true) { // TODO revert to while !!!
		memory->active = true;
#if MainLogger
		debug::log::mainLogger->info("playing version: {}", version);
#endif

		std::cout << "playing Generational Games:" << std::endl;

		sprintf(nameBuff, "logs/games/game_%d_Generator.gameLog", iteration);
		playGames(game, bestAgent, bestAgent, memory, probabilitic_moves, EPOCHS, nameBuff);
		std::cout << "memory size is: " << memory->memory.size() << std::endl;
		if (memory->memory.size() > memory_size) {
#if ProfileLogger
			debug::Profiler::profiler.switchOperation(5);
#endif
			currentAgent->fit(memory, iteration);
#if ProfileLogger
			debug::Profiler::profiler.stop();
#endif
			memory->active = false;
			std::cout << "playing Tournement Games:" << std::endl;

			sprintf(nameBuff, "logs/games/game_%d_Turney.gameLog", iteration);
			auto score = playGames(game, bestAgent, currentAgent, memory, Turnement_probabiliticMoves, TurneyEpochs, nameBuff);

			std::cout << "Turney ended with: " << score[currentAgent->identity] << " : " << score[bestAgent->identity] << std::endl;
			if (score[currentAgent->identity] > score[bestAgent->identity] * scoringThreshold) {
				version++;
				//TODO copy model weightsk
				currentAgent->model->save_as_current();
				bestAgent->model->copyModel(currentAgent->model);
				bestAgent->model->save_version(version);

				memory->save();
			}
		}
		iteration++;
	}
}

std::unordered_map<int, int> AlphaZero::ai::playGames(std::shared_ptr<Game::Game> game, std::shared_ptr<Agent> agent1, std::shared_ptr<Agent> agent2, std::shared_ptr<Memory> memory, int probMoves, int Epochs, char RunId[], int _goesFist)
{
	int goesFist = (_goesFist == 0) ? 1 : _goesFist;
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(3);
#endif
	std::unordered_map<int, int> scores;
	if (!agent1->identity == agent2->identity) {
		scores.insert({ agent1->identity, 0 });
		scores.insert({ agent2->identity, 0 });
	}

#if SaverType == 1
	io::FullState::GameSaver saver = io::FullState::GameSaver();
#elif SaverType == 2
	io::ActionsOnly::GameSaver saver = io::ActionsOnly::GameSaver();
#endif

	for (int epoch = 0; epoch < Epochs; epoch++) {
#if RenderGenAndTurneyProgress
		jce::consoleUtils::render_progress_bar((float)epoch / (float)Epochs);
#endif
#if ProfileLogger
		debug::Profiler::profiler.switchOperation(3);
#endif
#if	SaverType == 2 || SaverType == 1
		saver.addGame();
#endif
#if MainLogger
		if (epoch == 0) {
			debug::log::mainLogger->info("================================================================");
			debug::log::mainLogger->info("====================== playing Next match ======================");
			debug::log::mainLogger->info("================================================================");
		}
#endif
		if (_goesFist == 0) 
		{
			goesFist = -goesFist;
		}

		std::unordered_map<int, std::shared_ptr<Agent>> players = {
			{goesFist, agent1},
			{-goesFist,agent2}
		};
		agent1->getTree()->reset();
		agent2->getTree()->reset();

#if MainLogger
		if (epoch == 0) {
			debug::log::mainLogger->info("player {} will start", goesFist);
		}
#endif
		game->reset();
		int turn = 0;
		while (!game->state->done) {
			turn++;
			auto actionData = players[game->state->player]->getAction(game->state, probMoves > turn);
			memory->commit(game->state, actionData.second);
#if SaverType == 1
			saver.addState(game->state);
#elif SaverType == 2
			saver.addState(actionData.first);
#endif
#if MainLogger
			if (epoch == 0) {
				game->state->render(debug::log::mainLogger);
				debug::log::mainLogger->info("MSCT vals:");
				debug::log::logVector(debug::log::mainLogger, actionData.second);
				debug::log::mainLogger->info("NN vals:");
				debug::log::logVector(debug::log::mainLogger, players[game->state->player]->predict(game->state).second);
				debug::log::mainLogger->info("selected action is: {}", actionData.first);
			}
#endif
			game->takeAction(actionData.first);
		}
		// memory->commit(game->state);   add end game states to memory ??
#if SaverType == 1
		saver.addState(game->state);
#endif
#if MainLogger
		if (epoch == 0) {
			game->state->render(debug::log::mainLogger);
		}
#endif
#if ProfileLogger
		debug::Profiler::profiler.switchOperation(4);
#endif
		memory->updateMemory(game->state->player, std::get<0>(game->state->val));
		if (!agent1->identity == agent2->identity)
		{
			if (std::get<0>(game->state->val) != 0)
			{
				scores[players[game->state->player * std::get<0>(game->state->val)]->identity] += 1;
			}
		}
#if ProfileLogger
		debug::Profiler::profiler.stop();
#endif
	}

#if	SaverType == 2 || SaverType == 1
	saver.save(RunId);
#endif

#if RenderGenAndTurneyProgress
	jce::consoleUtils::render_progress_bar(1.0f, true);
#endif
	return scores;
}

// https://github.com/JulianWww/AlphaZero/
