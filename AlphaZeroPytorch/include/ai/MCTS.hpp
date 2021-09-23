#pragma once

#include <ai/model.hpp>


namespace AlphaZero {
	namespace ai {
		class Edge;
		class Node {
		public: std::shared_ptr<Game::GameState> state;
		public: std::unordered_map<int, std::shared_ptr<Edge>> edges;
		public: Node(std::shared_ptr<Game::GameState>);
		public: bool isLeaf();
		};

		class Edge {
			// I think thise are the right types
		public: int N=0;
		public: int action=0;
		public: float W=0;
		public: float P=0;
		public: float Q=0;
		public: std::shared_ptr<Node> outNode;
		public: std::shared_ptr<Node> inNode;
		public: Edge(std::shared_ptr<Node>outNode, std::shared_ptr<Node> inNode, int action, float p);
		};

		class MCTS {
		private: std::unordered_map<IDType, std::shared_ptr<Node>> MCTS_tree;

		public: MCTS(std::shared_ptr<Game::GameState>);
		public: float cpuct = cpuct_;
		public: std::pair < std::shared_ptr<AlphaZero::ai::Node>, std::list<std::shared_ptr<AlphaZero::ai::Edge>>> moveToLeaf(std::shared_ptr<Node>, int);
		public: void backFill(std::list<std::shared_ptr<Edge>>&, std::shared_ptr<Node> leaf, float val);
		public: std::shared_ptr<Node> getNode(IDType);
		public: std::shared_ptr<Node> addNode(std::shared_ptr<Game::GameState> state);
		public: void reset();
		};
	}
}

inline bool AlphaZero::ai::Node::isLeaf()
{
	return this->edges.size() == 0;
}

inline std::shared_ptr<AlphaZero::ai::Node> AlphaZero::ai::MCTS::addNode(std::shared_ptr<Game::GameState> state)
{
	if (this->MCTS_tree.count(state->id()) == 0) {
		std::shared_ptr<Node> newNode = std::make_shared<Node>(state);
		this->MCTS_tree.insert({ state->id(),  newNode });
		return newNode;
	}
	else {
		return this->MCTS_tree[state->id()];
	}
}

inline void AlphaZero::ai::MCTS::reset()
{
	this->MCTS_tree.clear();
}

inline std::shared_ptr<AlphaZero::ai::Node> AlphaZero::ai::MCTS::getNode(IDType key)
{
	return this->MCTS_tree[key];
}
