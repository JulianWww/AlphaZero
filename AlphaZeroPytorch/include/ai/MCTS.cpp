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

std::pair<AlphaZero::ai::Node*, std::list<AlphaZero::ai::Edge*>> AlphaZero::ai::MCTS::moveToLeaf(Node* node, int randomMovesRemaining)
{
	std::list<Edge*> edgeTrackList;
	while (true) {
		if (node->isLeaf()) {
			//must be the case since updateLock only propergates backward if somthing changed
			node->locked = false;
			node->updateLock();
			node->locked = true; // only if edges are none 
			return { node, edgeTrackList };
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
			float U;
			int idx = 0;
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
				if (!iter.second.outNode->locked)
				{
					U = U_computation(iter.second);
					if (nothasQU || U + iter.second.Q > maxQu) {
						opsEdge = &(iter.second);
						opsAction = iter.first;
						maxQu = U + iter.second.Q;
						nothasQU = false;
					}
				}
			}
			opsEdge->traverse();
			edgeTrackList.push_back(opsEdge);
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

void AlphaZero::ai::MCTS::backFill(WorkerData* trace)
{
	float currentPlayer = (float)trace->node->state->player * trace->value;

	for (auto edge : trace->edges)
	{
		edge->W = edge->W + currentPlayer * currentPlayer * edge->inNode->state->player;
		edge->Q = edge->W / (float)(edge->N);
	}
}
