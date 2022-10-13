#pragma once

#include <ai/agent.hpp>
#include <ai/memory.hpp>


namespace AlphaZero {
	namespace ai {
    // class containing game data to avoid using tuples
		struct gameOutput
		{
			std::unordered_map<Agent*, int> map;
			std::mutex ex;
			gameOutput(Agent*, Agent*);
			void updateValue(Agent*);
		};
      // run training loop consisting of loading and setting up everything, generating data, training, and tourney
		void train(int);

      // play games between players
		void playGames(gameOutput* output, Agent* agent1, Agent* agent2, Memory* memory, int probMoves, int Epochs, char RunId[], int goesFist = 0, bool log = false);

      // create threads that run the playGames function
		std::unordered_map<Agent*, int> playGames_inThreads(Game::Game* game, Agent* agent1, Agent* agend2, Memory* memory, int probMoves, int Epochs, int Threads, char RunId[], int goesFirst = 0, bool log = false);
	}
	namespace test {
    // run testing Game
		void playGame(std::shared_ptr<Game::Game> game, std::shared_ptr<ai::Agent> player1, std::shared_ptr<ai::Agent>player2, int goesFirst=0);
	}
}

inline std::unordered_map<AlphaZero::ai::Agent*, int> AlphaZero::ai::playGames_inThreads(Game::Game* game, Agent* agent1, Agent* agent2, Memory* memory, int probMoves, int Epochs, int Threads, char RunId[], int goesFirst, bool log)
{
	gameOutput output(agent1, agent2);

	std::vector<std::thread> workers;
	for (size_t idx = 0; idx < Threads; idx++)
	{
		bool doLog = (log && (idx == 0));
		workers.push_back(std::thread(playGames, &output, agent1, agent2, memory, probMoves, Epochs, RunId, goesFirst, doLog));
	}

	for (auto& worker : workers)
	{
		worker.join();
	}

	return output.map;
}

inline void AlphaZero::ai::gameOutput::updateValue(Agent* idx)
{
	this->ex.lock();
	this->map[idx] = this->map[idx] + 1;
	this->ex.unlock();
}

inline AlphaZero::ai::gameOutput::gameOutput(Agent* agent1, Agent* agent2)
{
	this->map.insert({ agent1, 0 });
	this->map.insert({ agent2, 0 });
}