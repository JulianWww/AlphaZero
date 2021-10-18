#include "MCTS.hpp"
#include <limits>
#include <random/dirichlet.hpp>
#include <jce/vector.hpp>

std::random_device rd;
std::mt19937 gen(rd());

AlphaZero::ai::Node::Node(std::shared_ptr<Game::GameState> state)
{
	this->state = state;
}

AlphaZero::ai::Edge::Edge(std::shared_ptr<Node> _outNode, Node* _inNode, int _action, float _p)
{
	this->P = _p;
	this->action = _action;
	this->outNode = _outNode;
	this->inNode = _inNode;
	this->N = 0;
	this->W = 0;
	this->Q = 0;
}

AlphaZero::ai::MCTS::MCTS(std::shared_ptr<Game::GameState> root)
{
	this->addNode(root);
}

std::pair<AlphaZero::ai::Node*, std::list<AlphaZero::ai::Edge*>> AlphaZero::ai::MCTS::moveToLeaf(Node* node, int randomMovesRemaining)
{
	std::list<Edge*> backTrackList;
	while (true) {
		if (node->isLeaf()) {
			return { node, backTrackList };
		}
		else {
			int doRand = 0;
			std::vector<double> nu;
			if (randomMovesRemaining > 0) {
				doRand = 1;
				randomMovesRemaining = randomMovesRemaining - 1;
				dirichlet_distribution<std::mt19937> d(jce::vector::gen<double>(node->edges.size(), Alpha));
				nu = d(gen);
			}
			else {
				nu = jce::vector::gen<double>(node->edges.size(), 0);
			}
			float maxQu = MaxQuDefault;
			float U;
			int idx = 0;
			int Nb = 0;
			for (auto const& iter : node->edges) {
				Nb += iter.second->N;
			}

			Edge* opsEdge;
			int opsAction;

			
			for (auto const& iter : node->edges) {
				U = U_computation(iter.second);
				if (U + iter.second->Q > maxQu) {
					opsEdge = iter.second.get();
					opsAction = iter.first;
					maxQu = U + iter.second->Q;
				}
			}
			opsEdge->traverse();
			backTrackList.push_back(opsEdge);
			node = opsEdge->outNode.get();;
		}
	}
}

void AlphaZero::ai::MCTS::backFill(std::list<Edge*>& backTrace, Node* leaf, float val)
{
	int& currentPlayer = leaf->state->player;

	for (auto const& edge : backTrace) {
		edge->inNode->lock.lock();

		edge->W = edge->W + val * currentPlayer * edge->inNode->state->player;
		edge->Q = edge->W / edge->N;

		edge->inNode->lock.unlock();
	}
}

