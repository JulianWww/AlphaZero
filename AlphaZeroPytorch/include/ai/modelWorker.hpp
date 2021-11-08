#pragma once

#include "MCTS.hpp"

namespace AlphaZero
{
	namespace ai
	{
		class Node;

		class ModelData
		{
		public: Node* node;
		public: torch::Tensor polys;
		public: float value;

		public: ModelData(Node* node);
		public: void print();
		};
	}
}

inline AlphaZero::ai::ModelData::ModelData(Node* _node)
{
	this->node = _node;
}