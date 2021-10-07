#pragma once
#include <ai/MCTS.hpp>
#include <ai/memory.hpp>
#include <jce/vector.hpp>
#include "utils.hpp"

namespace AlphaZero {
	namespace ai {
		class Agent {
		public: std::shared_ptr<MCTS> tree;
		public: std::shared_ptr<Model> model;
		public: Agent(std::shared_ptr<Game::Game> game);
		public: int identity;
#if Training
		public: std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::GameState> state, bool proabilistic);
#else
		public: virtual std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::GameState> state, bool proabilistic);
#endif
		public: void runSimulations(Node*);
		private: float evaluateLeaf(Node*);
		public: void fit(std::shared_ptr<Memory> memory, unsigned short iteration);
		private: std::pair<float, std::vector<float>> predict(std::shared_ptr<Game::GameState> state);
		private: std::pair<int, std::vector<float>> derministicAction(Node* node);
		private: std::pair<int, std::vector<float>> prabilisticAction(Node* node);
		};
#if not Training
		class User : public Agent {
		public: virtual std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::Game> game, bool proabilistic);
		};
#endif
		void runSimulationsCaller(AlphaZero::ai::Agent* agent, Node*);
	}
}

inline void AlphaZero::ai::Agent::runSimulations(Node* node)
{
	auto serchResults = this->tree->moveToLeaf(node, ProbabiliticMoves);
	auto val = this->evaluateLeaf(serchResults.first);
	this->tree->backFill(serchResults.second, serchResults.first, val);

	this->tree->addMCTSIter();
}

inline void AlphaZero::ai::runSimulationsCaller(AlphaZero::ai::Agent* agent, Node* node)
{
	while (agent->tree->MCTSIter < MCTSSimulations) {
		agent->runSimulations(node);
	}
}

inline float AlphaZero::ai::Agent::evaluateLeaf(Node* node)
{
	if (!node->state->done){
		std::shared_ptr<Game::GameState> nextState;
		Node* nextNode;
		auto NNvals = this->predict(node->state);
		for (auto& action : node->state->allowedActions) {
			nextState = node->state->takeAction(action);
			nextNode = this->tree->addNode(nextState);
			std::shared_ptr<Edge> newEdge = std::make_shared<Edge>(nextNode, node, action, NNvals.second[action]); //the last is the prob
			node->addEdge( action, newEdge);
		}
		return NNvals.first;
	}
	return (float)std::get<2>(node->state->val);
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

inline std::pair<float, std::vector<float>> AlphaZero::ai::Agent::predict(std::shared_ptr<Game::GameState> state)
{
	auto preds = this->model->predict(state);
	float& val = preds.first;
	std::vector<float> polys = std::vector<float>(action_count);

	for (unsigned short idx = 0; idx < action_count; idx++) {
		polys[idx] = preds.second[0][idx].item<float>();
	}
	return { val, polys };
}

inline std::pair<int, std::vector<float>> AlphaZero::ai::Agent::derministicAction(Node* node)
{
	int action = 0;
	unsigned int max_N = 0;
	unsigned int sum = 0;
	std::vector<float> probs = jce::vector::gen(action_count, 0.0f);
	for (auto const& iter : node->edges) {
		if (iter.second->N > max_N) {
			max_N = iter.second->N;
			action = iter.first;
		}
		probs[iter.second->action] = ((float)iter.second->N);
	}
	linmax(probs);
	return { action, probs };
}

inline std::pair<int, std::vector<float>> AlphaZero::ai::Agent::prabilisticAction(Node* node)
{
	int action = -1;
	int idx = 0;
	float action_probs = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	unsigned int sum = 0;
	std::vector<float> probs = jce::vector::gen(action_count, 0.0f);

	for (auto const& iter : node->edges) {
		probs[iter.second->action] = ((float)iter.second->N);
	}
	linmax(probs);

	float sumF = (float)sum;
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
