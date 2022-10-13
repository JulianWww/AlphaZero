#pragma once
#include <mutex>
#include <game/game.hpp>
#include <jce/vector.hpp>

// remove one mutex - the Node mutex


namespace AlphaZero {
	namespace ai {
		class Node;

		/*Class Represienting the action connecting 2 nodes togather. It handles all The MCTS relevant variables and the mutex for
		* parallization
		*/
		class Edge {
			// The number of times the Edge was traversed
		public: int N = 0;
			  // the probability initialized by the NN
		public: float P = 0;
			  // the action asociated with the action
		public: int action = 0;
			  // the amount of times this lead to a win
		public: float W = 0;
			  // the win probability
		public: float Q = 0;

      // node pointing from
		public: Node* outNode;

      // node pointing to
		public: Node* inNode;

		public: Edge(Node* outNode, Node* inNode, int action, float p);
		public: Edge();

      // call N++
		public: void traverse();
		};

		class Node {
       
       // pointer to the asociated game state
		public: std::shared_ptr<Game::GameState> state;

      // hashmap of actions to edges
		public: std::unordered_map<int, Edge> edges;
		public: Node(std::shared_ptr<Game::GameState>);

      // returns edges.size() == 0
		public: bool isLeaf();

     // add an edge tot the Node
		public: void addEdge(int id, Edge& edge);
		};

		std::vector<float> getQ(Node*);

		class MCTS {
      // iteration counter
		public: unsigned short MCTSIter = 0;

      // hash map containing the nodes using their states id's as keys
		private: std::unordered_map<IDType, std::unique_ptr<Node>> MCTS_tree;

      // MCTSIter ++
		public: void addMCTSIter();
		public: MCTS();

      // configurational constant cpuct;
		public: float cpuct = cpuct_;

      // find leaf node from root node using the selection descriped in the paper
		public: std::pair <Node*, std::list<Edge*>> moveToLeaf(Node*);

      // update all traversed edges
		public: void backFill(std::list<Edge*>&, Node* leaf, float val);

      // get node by ID hash
		public: Node* getNode(IDType);

      // add a node if necaray and call getNode;
		public: Node* addNode(std::shared_ptr<Game::GameState> state);

      // reset to original state;
		public: void reset();
		};
	}
}

inline std::vector<float> AlphaZero::ai::getQ(Node* node)
{
	std::vector<float> data = jce::vector::gen(42, 0.0f);
	for (auto const& pos : node->edges)
	{
		data[pos.first] = pos.second.Q;
	}
	return data;
}

inline void AlphaZero::ai::MCTS::addMCTSIter()
{
	MCTSIter++;
}

inline bool AlphaZero::ai::Node::isLeaf()
{
	return this->edges.size() == 0;
}

inline void AlphaZero::ai::Node::addEdge(int id, Edge& edge)
{
	this->edges.insert({ id, edge });
}

inline AlphaZero::ai::MCTS::MCTS(){}

inline AlphaZero::ai::Node* AlphaZero::ai::MCTS::addNode(std::shared_ptr<Game::GameState> state)
{
	if (this->MCTS_tree.count(state->id()) == 0) {

		this->MCTS_tree.insert({ state->id(), std::make_unique<Node>(state)});
	}
	return this->getNode(state->id());
}

inline void AlphaZero::ai::MCTS::reset()
{	
	this->MCTS_tree.clear();
}

inline AlphaZero::ai::Node* AlphaZero::ai::MCTS::getNode(IDType key)
{
	return this->MCTS_tree[key].get();
}


inline void AlphaZero::ai::Edge::traverse()
{
	this->N++;
}