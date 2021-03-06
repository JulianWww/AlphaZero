#include "MCTS.hpp"
#include <limits>
#include <jce/vector.hpp>

AlphaZero::ai::Node::Node(std::shared_ptr<Game::GameState> state)
{
	this->state = state;
}

AlphaZero::ai::Edge::Edge(Node* _outNode, Node* _inNode, int _action, float _p)
{
	this->P = _p;
	this->action = _action;
	this->outNode = _outNode;
	this->inNode = _inNode;
	this->N = 0;
	this->W = 0;
	this->Q = 0;
}

AlphaZero::ai::Edge::Edge()
{
	std::cout << "Edge default constructor" << std::endl;
	return;
}

std::pair<AlphaZero::ai::Node*, std::list<AlphaZero::ai::Edge*>> AlphaZero::ai::MCTS::moveToLeaf(Node* node)
{
	std::list<Edge*> backTrackList;
	while (true) {
		if (node->isLeaf()) {
			return { node, backTrackList };
		}
		else {
			float U;
			int Nb = 0;
			for (auto const& iter : node->edges) 
			{
				Nb += iter.second.N;
			}

			Edge* opsEdge = &(node->edges.begin()->second);
			int opsAction;

			float maxQu;
			bool nothasQU = true;
			
			for (auto& iter : node->edges) {
				U = U_computation(iter.second);
				if (nothasQU || U + iter.second.Q > maxQu) {
					opsEdge = &(iter.second);
					opsAction = iter.first;
					maxQu = U + iter.second.Q;
					nothasQU = false;
				}
			}
			opsEdge->traverse();
			backTrackList.push_back(opsEdge);
			node = opsEdge->outNode;
		}
	}
}

void AlphaZero::ai::MCTS::backFill(std::list<Edge*>& backTrace, Node* leaf, float val)
{
	float currentPlayer = (float)leaf->state->player * val;

	for (auto const& edge : backTrace) {
		// edge->inNode->lock.lock();

		edge->W = edge->W + currentPlayer * (float)edge->inNode->state->player;
		edge->Q = edge->W / (float)edge->N;

		// edge->inNode->lock.unlock();
	}
}

