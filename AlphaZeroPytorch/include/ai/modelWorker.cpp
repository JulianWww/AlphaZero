#include "modelWorker.hpp"
#include "MCTS.hpp"

void AlphaZero::ai::ModelData::print()
{
	std::cout << "state: " << std::endl;
	this->node->state->render();
	std::cout << "polys: " << std::endl;
	std::cout << this->polys << std::endl;
	std::cout << "value: " << this->value << std::endl;
}