#pragma once

#include <ai/agent.hpp>
#include <ai/memory.hpp>


namespace AlphaZero {
	namespace ai {
		void train(int);
		std::unordered_map<int, int> playGames(std::shared_ptr<Game::Game> game, std::shared_ptr<Agent> agent1, std::shared_ptr<Agent>agent2, std::shared_ptr<Memory> memory, int probMoves, int Epochs, char RunId[], int goesFist = 0);
	}
	namespace test {
		void playGame(std::shared_ptr<Game::Game> game, std::shared_ptr<ai::Agent> player1, std::shared_ptr<ai::Agent>player2, int goesFirst=0);
	}
}