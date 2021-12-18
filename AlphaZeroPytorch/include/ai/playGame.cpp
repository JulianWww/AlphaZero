#include "playGame.hpp"
#include <jce/load.hpp>
#include <jce/save.hpp>
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
	std::vector<char*> devices = { DEVICES };
	std::shared_ptr<Memory> memory = std::make_shared<Memory>();
	std::shared_ptr<Game::Game> game = std::make_shared<Game::Game>();
	std::shared_ptr<Agent> currentAgent = std::make_shared<Agent>(devices);
	std::shared_ptr<Agent> bestAgent = std::make_shared<Agent>(devices);

	std::vector<int> requiredIterations;

	memory->load();
	char nameBuff[100];

	currentAgent->identity = 0;
	bestAgent->identity = 1;

	std::sprintf(nameBuff, "models/run_%d/versionCount.jce", runVersion);
	std::ifstream fin(nameBuff, std::ios::binary);
	if (fin.is_open())
	{
		jce::load(fin, version);
		std::cout << "found model version: " << version << std::endl;

		fin.close();

		std::sprintf(nameBuff, "models/run_%d/iterationCounter.jce", runVersion);
		fin.open(nameBuff, std::ios::binary);
		if (fin.is_open())
		{
			jce::load(fin, requiredIterations);
			std::cout << "loaded required Iterations: it hs size: " << requiredIterations.size() << std::endl;
		}
		else
		{
			std::cout << "could not find sutalbe iterationCounter";
		}
		fin.close();

		bestAgent->model->load_version(version);
		currentAgent->model->load_version(version);
	}
	else
	{
		std::cout << "model version config not found. Defaulting to 0" << std::endl;
		version = 0;
	}
	currentAgent->model->copyModel(bestAgent->model.get());

	// TODO bestAgent->model->save(0);
	while (true) { // TODO revert to while !!!
		iteration++;
		memory->active = true;
#if MainLogger
		debug::log::mainLogger->info("playing version: {}", version);
#endif

		std::cout << "playing Generational Games:" << std::endl;

#if ModelLogger
		debug::log::modelLogger->info("Running Training Games");
#endif
		
		sprintf(nameBuff, "logs/games/game_%d_Generator.gameLog", iteration);
		playGames_inThreads(game.get(), bestAgent.get(), bestAgent.get(), memory.get(), probabilitic_moves, EPOCHS, GEN_THREADS, nameBuff, 1, false);
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
#if MainLogger
			debug::log::mainLogger->info("RETRAINING =========================================================");
#endif

			sprintf(nameBuff, "logs/games/game_%d_Turney.gameLog", iteration);
#if ModelLogger
			debug::log::modelLogger->info("Running Tourney Games");
#endif
			auto score = playGames_inThreads(game.get(), bestAgent.get(), currentAgent.get(), memory.get(), Turnement_probabiliticMoves, TurneyEpochs, TurneyThreads, nameBuff, 0, true);

			std::cout << "Turney ended with: " << score[currentAgent.get()] << " : " << score[bestAgent.get()] << std::endl;
			if (score[currentAgent.get()] > score[bestAgent.get()] * scoringThreshold) {
				version++;
				//TODO copy model weightsk
				currentAgent->model->save_as_current();
				bestAgent->model->copyModel(currentAgent->model.get());
				bestAgent->model->save_version(version);

				std::sprintf(nameBuff, "models/run_%d/versionCount.jce", runVersion);
				std::ofstream fout(nameBuff, std::ios::binary);
				jce::save(fout, version);
				fout.close();

				memory->save();

				requiredIterations.push_back(iteration);
				std::sprintf(nameBuff, "models/run_%d/iterationCounter.jce", runVersion);
				fout.open(nameBuff, std::ios::binary);
				jce::save(fout, requiredIterations);
				fout.close();

				iteration = 0;
			}
		}
	}
}

void AlphaZero::ai::playGames(gameOutput* output, Agent* agent1, Agent* agent2, Memory* memory, int probMoves, int Epochs, char RunId[], int _goesFist, bool do_log)
{
	std::srand(std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now()).time_since_epoch().count());
	auto game = std::make_unique<Game::Game>();
	int goesFist = (_goesFist == 0) ? 1 : _goesFist;
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(3);
#endif

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
		if (epoch == 0 && do_log) {
			debug::log::mainLogger->info("================================================================");
			debug::log::mainLogger->info("====================== playing Next match ======================");
			debug::log::mainLogger->info("================================================================");
		}
#endif
		if (_goesFist == 0) 
		{
			goesFist = -goesFist;
		}

		std::unordered_map<int, Agent*> players = {
			{goesFist, agent1},
			{-goesFist,agent2}
		};
		agent1->getTree()->reset();
		agent2->getTree()->reset();

		auto tmpMemory = memory->getTempMemory();

#if MainLogger
		if (epoch == 0 && do_log) 
		{
			debug::log::mainLogger->info("player {} will start", goesFist);
		}
#endif
		game->reset();
		int turn = 0;
		while (!game->state->done) {
			turn++;
			//std::cout << turn << std::endl;
			auto actionData = players[game->state->player]->getAction(game->state, probMoves > turn);
			tmpMemory.commit(game->state, actionData.second.first);
#if SaverType == 1
			saver.addState(game->state);
#elif SaverType == 2
			saver.addState(actionData.first);
#endif
#if MainLogger
			if (epoch == 0 && do_log) {
				game->state->render(debug::log::mainLogger);
				debug::log::mainLogger->info("MSCT vals: {:1.5f}", actionData.second.second);
				debug::log::logVector(debug::log::mainLogger, actionData.second.first);
				debug::log::mainLogger->info("NN vals: {:1.5f}", players[game->state->player]->predict(game->state).first);
				debug::log::logVector(debug::log::mainLogger, players[game->state->player]->predict(game->state).second);

				debug::log::mainLogger->info("selected action is: {}", actionData.first);
			}
#endif
			//game->render();
			game->takeAction(actionData.first);
		}
		//std::cout << turn << std::endl;
		//game->render();
		//tmpMemory.commit(game->state);//   add end game states to memory ??
#if SaverType == 1
		saver.addState(game->state);
#endif
#if MainLogger
		if (epoch == 0 && do_log) {
			game->state->render(debug::log::mainLogger);
		}
#endif
#if ProfileLogger
		debug::Profiler::profiler.switchOperation(4);
#endif
		memory->updateMemory(game->state->player, std::get<0>(game->state->val), &tmpMemory);
		if (true)
		{
			if (std::get<0>(game->state->val) != 0)
			{
				output->updateValue(players[game->state->player * std::get<0>(game->state->val)]);
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
}