#include <ai/model.hpp>
#include <ai/agent.hpp>
#include "testUtils.hpp"

namespace AlphaZero
{
	namespace test
	{
		void runTests();

		void testCoppying();
		void testSave();
		void testJCESave();
		void testLossLog();
		void testModelData();
		void testTraining();
		void testModelSpeed();
		void testModelSyncronization();

		bool compareAgents(std::shared_ptr<ai::Agent> anget1, std::shared_ptr<ai::Agent> anget2);
		std::shared_ptr<Game::GameState> getRandomState();
	}
}

inline bool AlphaZero::test::compareAgents(std::shared_ptr<ai::Agent> anget1, std::shared_ptr<ai::Agent> anget2)
{
	auto state = getRandomState();

	auto valsA = anget1->predict(state);
	auto valsB = anget2->predict(state);

	if (valsA.first != valsB.first) { return false; }
	for (size_t idx = 0; idx < action_count; idx++)
	{
		if (valsA.second[idx] != valsB.second[idx]) { return false; }
	}
	return true;
}


inline std::shared_ptr<AlphaZero::Game::GameState> AlphaZero::test::getRandomState()
{
	std::bitset<stateSize> board;
	for (size_t idx = 0; idx < stateSize; idx++)
	{
		board.set(idx, rand() % 2);
	}
	auto state = std::make_shared<Game::GameState>(board, rand() % 2);
	return state;
}