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
		public: std::unordered_map<int, Edge> edges;
		public: Node(std::shared_ptr<Game::GameState>);
		public: bool isLeaf();
		public: void addEdge(int id, Edge& edge);
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
		public: Node* outNode;
		public: Node* inNode;
		public: Edge(Node* outNode, Node* inNode, int action, float p);
		public: void traverse();
		};

		class MCTS {
			// mutex keeping corrupion within the Tree from occuring when new nodes are added
		// public: std::mutex NodeInsersionMutex;
			// mutex keeping corrupion of the MCTSIter variable
		// public: std::mutex MCTSIterMutex;
		public: unsigned short MCTSIter = 0;
		private: std::unordered_map<IDType, std::unique_ptr<Node>> MCTS_tree;

			   // add 1 to MCTSIter within a mutex
		public: void addMCTSIter();
		public: MCTS(std::shared_ptr<Game::GameState>);
		public: float cpuct = cpuct_;
		public: std::pair <Node*, std::list<Edge*>> moveToLeaf(Node*, int);
		public: void backFill(std::list<Edge*>&, Node* leaf, float val);
		public: Node* getNode(IDType);
		public: Node* addNode(std::shared_ptr<Game::GameState> state);
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

inline void AlphaZero::ai::Node::addEdge(int id, Edge& edge)
{
	// this->lock.lock();
	this->edges.insert({ id, edge });
	// this->lock.unlock();
}

inline AlphaZero::ai::Node* AlphaZero::ai::MCTS::addNode(std::shared_ptr<Game::GameState> state)
{
	if (this->MCTS_tree.count(state->id()) == 0) {

		// this->NodeInsersionMutex.lock();
		this->MCTS_tree.insert({ state->id(), std::make_unique<Node>(state)});
		// this->NodeInsersionMutex.unlock();
	}
	return this->getNode(state->id());
}

inline void AlphaZero::ai::MCTS::reset()
{
	// this->NodeInsersionMutex.lock();
	this->MCTS_tree.clear();
	// this->NodeInsersionMutex.unlock();
}

inline AlphaZero::ai::Node* AlphaZero::ai::MCTS::getNode(IDType key)
{
	return this->MCTS_tree[key].get();
}


inline void AlphaZero::ai::Edge::traverse()
{
	// this->inNode->lock.lock();
	this->N++;
	// this->inNode->lock.unlock();
}