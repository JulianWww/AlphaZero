#include <ai/model.hpp>
#include <ai/agent.hpp>
#include <iostream>

namespace AlphaZero
{
	namespace test
	{
		void runTests();

		void testCoppying();
		void testSave();
		void testLossLog();

		bool compareAgents(std::shared_ptr<ai::Agent> anget1, std::shared_ptr<ai::Agent> anget2);
		void printSuccess(bool val);
	}
}

inline bool AlphaZero::test::compareAgents(std::shared_ptr<ai::Agent> anget1, std::shared_ptr<ai::Agent> anget2)
{
	std::bitset<stateSize> board;
	for (size_t idx = 0; idx < stateSize; idx++)
	{
		board.set(idx, rand() % 2);
	}
	auto state = std::make_shared<Game::GameState>(board, rand() % 2);

	auto valsA = anget1->model->workers.front()->model->predict(state, 
		c10::Device(anget1->model->workers.front()->deviceName));
	auto valsB = anget2->model->workers.front()->model->predict(state, 
		c10::Device(anget2->model->workers.front()->deviceName));

	if (valsA.first != valsB.first) { return false; }
	for (size_t idx = 0; idx < action_count; idx++)
	{
		if (valsA.second[0][idx].item<float>() != valsB.second[0][idx].item<float>()) 
		{ 
			return false; 
		}
	}
	return true;
}

inline void AlphaZero::test::printSuccess(bool val)
{
	if (val)
	{
		std::cout << "\33[32;1mSuccess\33[0m" << std::endl;
	}
	else
	{
	std::cout << "\33[31;1mFailed\33[0m" << std::endl;
	}
}