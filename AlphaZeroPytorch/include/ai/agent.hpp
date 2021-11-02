#pragma once
#include <ai/MCTS.hpp>
#include <ai/modelWorker.hpp>
#include <jce/vector.hpp>
#include "utils.hpp"

namespace AlphaZero {
	namespace ai {
		class Agent {
		public: std::shared_ptr<MCTS> tree;
		public: std::shared_ptr<modelManager> model;
		public: Agent(std::vector<char*> devices);
		public: int identity;
#if Training
		public: std::pair<int, std::vector<int>> getAction(std::shared_ptr<Game::GameState> state, bool proabilistic);
#else
		public: virtual std::pair<int, std::vector<int>> getAction(std::shared_ptr<Game::GameState> state, bool proabilistic);
#endif
		public: void runSimulations(Node*);
		private: void evaluateLeaf(WorkerData*);
		public: void fit(std::shared_ptr<Memory> memory, unsigned short iteration);
		public: void predict(std::pair<Node*, std::list<Edge*>> state);
		private: std::pair<int, std::vector<int>> derministicAction(Node* node);
		private: std::pair<int, std::vector<int>> prabilisticAction(Node* node);
		};
#if not Training
		class User : public Agent {
		public: virtual std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::Game> game, bool proabilistic);
		};
#endif
		void runSimulationsCaller(AlphaZero::ai::Agent* agent, Node* node);
	}
}

inline void AlphaZero::ai::Agent::runSimulations(Node* node)
{
	if (this->model->toRun.size() < NNMaxBatchSize && node->rootLocked() && this->tree->MCTSIter < MCTSSimulations && this->model->isDone())
	{
		auto serchResults = this->tree->moveToLeaf(node, ProbabiliticMoves);
		this->predict(serchResults);
		this->tree->addMCTSIter();
		std::cout << "gen" << std::endl;
	}
	if (this->model->hasRun.size() > 0)
	{
		auto predicted = this->model->getFinishedData();
		for (auto& state : predicted)
		{
			this->evaluateLeaf(state);
			this->tree->backFill(state);
			delete state;
			std::cout << "back" << std::endl;
		}
	}
}

inline void AlphaZero::ai::runSimulationsCaller(AlphaZero::ai::Agent* agent, Node* node)
{
	while (agent->tree->MCTSIter < MCTSSimulations || !agent->model->isDone()) {
		agent->runSimulations(node);
	}
}

inline void AlphaZero::ai::Agent::evaluateLeaf(WorkerData* trace)
{
	Node* node = trace->node;
	if (!node->state->done){
		Game::GameState* nextState;
		Node* nextNode;
		for (auto& action : node->state->allowedActions) {
			nextState = node->state->MCTS_takeAction(action);
			nextNode = this->tree->addNode(nextState);
			nextNode->inNodes.push_back(node);
			Edge newEdge = Edge(nextNode, node, action, trace->polys[action]); //the last is the prob
			node->addEdge( action, newEdge);
		}
	}
	node->locked = true;
	node->updateLock();
	node->locked = false;
	return ;
}

inline void AlphaZero::ai::Agent::fit(std::shared_ptr<Memory> memory, unsigned short run)
{
	std::cout << "\33[35;1mretraining\33[0m" << std::endl;
	for (int idx = 0; idx < Training_loops ; idx++) {
#if RenderTrainingProgress
		jce::consoleUtils::render_progress_bar((float)idx / (float)Training_loops);
#endif
		auto batch = Model::getBatch(memory, Training_batch);
		this->model->fit(batch, run, idx);
	}
#if RenderTrainingProgress
	jce::consoleUtils::render_progress_bar(1.0f, true);
#endif
}

inline void AlphaZero::ai::Agent::predict(std::pair<Node*, std::list<Edge*>> data)
{
	this->model->addNode(new WorkerData(data));
}

inline std::pair<int, std::vector<int>> AlphaZero::ai::Agent::derministicAction(Node* node)
{
	int action = 0;
	unsigned int max_N = 0;
	unsigned int sum = 0;
	std::vector<int> probs = jce::vector::gen(action_count, 0);
	for (auto const& iter : node->edges) {
		if (iter.second.N > max_N) {
			max_N = iter.second.N;
			action = iter.first;
		}
		probs[iter.second.action] = iter.second.N;
	}
	return { action, probs };
}

inline std::pair<int, std::vector<int>> AlphaZero::ai::Agent::prabilisticAction(Node* node)
{
	int action = -1;
	int idx = 0;
	unsigned int sum = 0;
	std::vector<int> probs = jce::vector::gen(action_count, 0);

	for (auto const& iter : node->edges) {
		probs[iter.second.action] = (iter.second.N);
	}

	//softMaxNoDiv(probs);
	int action_probs = rand() % getSumm(probs);
	for (auto const& val : probs) {
		action_probs -= val;
		if (action_probs < 0) {
			action = idx;
			break;
		}
		idx++;
	}
	return { action, probs };
}