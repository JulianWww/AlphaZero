#pragma once

#include <ai/model.hpp>
#include <mutex>

// remove one mutex - the Node mutex


namespace AlphaZero {
	namespace ai {
		class Edge;
		class Node {
			// mutex locking the during insersion of edges and also used as the child edges mutex
		//public: std::mutex lock;
		public: std::shared_ptr<Game::GameState> state;
		public: std::unordered_map<int, std::shared_ptr<Edge>> edges;
		public: Node(std::shared_ptr<Game::GameState>);
		public: bool isLeaf();
		public: void addEdge(int id, std::shared_ptr<Edge>& edge);
		};

		/*Class Represienting the action connecting 2 nodes togather. It handles all The MCTS relevant variables and the mutex for
		* parallization
		*/
		class Edge {
			  // The number of times the Edge was traversed
		public: int N=0;
			  // the action asociated with the action
		public: int action=0;
			  // the amount of times this lead to a win
		public: float W=0;
			  // the probability initialized by the NN
		public: float P=0;
			  // the win probability
		public: float Q=0;
		public: std::shared_ptr<Node> outNode;
		public: std::shared_ptr<Node> inNode;
		public: Edge(std::shared_ptr<Node> outNode, std::shared_ptr<Node> inNode, int action, float p);
		public: void traverse();
		};

		class MCTS {
			// mutex keeping corrupion within the Tree from occuring when new nodes are added
		// public: std::mutex NodeInsersionMutex;
			// mutex keeping corrupion of the MCTSIter variable
		// public: std::mutex MCTSIterMutex;
		public: unsigned short MCTSIter = 0;
		private: std::unordered_map<IDType, std::shared_ptr<Node>> MCTS_tree;

			   // add 1 to MCTSIter within a mutex
		public: void addMCTSIter();
		public: MCTS(std::shared_ptr<Game::GameState>);
		public: float cpuct = cpuct_;
		public: std::pair <std::shared_ptr<Node>, std::list<std::shared_ptr<Edge>>> moveToLeaf(std::shared_ptr<Node>, int);
		public: void backFill(std::list<std::shared_ptr<Edge>>&, std::shared_ptr<Node> leaf, float val);
		public: std::shared_ptr<Node> getNode(IDType);
		public: std::shared_ptr<Node> addNode(std::shared_ptr<Game::GameState> state);
		public: void reset();
		};
	}
}

inline void AlphaZero::ai::MCTS::addMCTSIter()
{
	// this->MCTSIterMutex.lock();
	MCTSIter++;
	// this->MCTSIterMutex.unlock();
}

inline bool AlphaZero::ai::Node::isLeaf()
{
	return this->edges.size() == 0;
}

inline void AlphaZero::ai::Node::addEdge(int id, std::shared_ptr<Edge>& edge)
{
	// this->lock.lock();
	this->edges.insert({ id, edge });
	// this->lock.unlock();
}

inline std::shared_ptr<AlphaZero::ai::Node> AlphaZero::ai::MCTS::addNode(std::shared_ptr<Game::GameState> state)
{
	if (this->MCTS_tree.count(state->id()) == 0) {
		auto newNode = std::make_shared<Node>(state);

		// this->NodeInsersionMutex.lock();
		this->MCTS_tree.insert({ state->id(),  newNode });
		// this->NodeInsersionMutex.unlock();

		return newNode;
	}
	else {
		return this->MCTS_tree[state->id()];
	}
}

inline void AlphaZero::ai::MCTS::reset()
{
	// this->NodeInsersionMutex.lock();
	this->MCTS_tree.clear();
	// this->NodeInsersionMutex.unlock();
}

inline std::shared_ptr<AlphaZero::ai::Node> AlphaZero::ai::MCTS::getNode(IDType key)
{
	return this->MCTS_tree[key];
}


inline void AlphaZero::ai::Edge::traverse()
{
	// this->inNode->lock.lock();
	this->N++;
	// this->inNode->lock.unlock();
}