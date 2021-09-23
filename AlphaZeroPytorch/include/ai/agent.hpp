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
		public: Agent(std::shared_ptr<Game::Game> game, int version);
#if Training
		public: std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::Game> game, bool proabilistic);
#else
		public: virtual std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::Game> game, bool proabilistic);
#endif
		public: void runSimulations(std::shared_ptr<Game::GameState>);
		private: float evaluateLeaf(std::shared_ptr<Node>&);
		public: void fit(std::shared_ptr<Memory> memory, unsigned short iteration);
		private: std::pair<float, std::vector<float>> predict(std::shared_ptr<Game::GameState> state);
		private: std::pair<int, std::vector<float>> derministicAction(std::shared_ptr<Node>const& node);
		private: std::pair<int, std::vector<float>> prabilisticAction(std::shared_ptr<Node>const& node);
		};
#if not Training
		class User : public Agent {
		public: virtual std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::Game> game, bool proabilistic);
		};
#endif
		void runSimulationsCaller(AlphaZero::ai::Agent* agent, std::shared_ptr<Game::GameState>);
	}
}
inline void AlphaZero::ai::Agent::runSimulations(std::shared_ptr<Game::GameState> state)
{
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(0);
#endif
	std::shared_ptr<Node> node = this->tree->addNode(state);
	auto serchResults = this->tree->moveToLeaf(node, ProbabiliticMoves);
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(1);
#endif
	auto val = this->evaluateLeaf(serchResults.first);
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(2);
#endif
	this->tree->backFill(serchResults.second, serchResults.first, val);
}

inline void AlphaZero::ai::runSimulationsCaller(AlphaZero::ai::Agent* agent, std::shared_ptr<Game::GameState> state)
{
	for (int i = 0; i < MCTSSimulations; i++) {
		agent->runSimulations(state);
	}
#if ProfileLogger
	debug::Profiler::profiler.switchOperation(3);
#endif
}

inline float AlphaZero::ai::Agent::evaluateLeaf(std::shared_ptr<Node>& node)
{
	if (!node->state->done){
		std::shared_ptr<Game::GameState> nextState;
		std::shared_ptr<Node> nextNode;
		auto NNvals = this->predict(node->state);
		for (auto& action : node->state->allowedActions) {
			nextState = node->state->takeAction(action);
			nextNode = this->tree->addNode(nextState);
			std::shared_ptr<Edge> newEdge = std::make_shared<Edge>(nextNode, node, action, NNvals.second[action]); //the last is the prob
			node->edges.insert({ action, newEdge });
		}
		return NNvals.first;
	}
	return (float)std::get<2>(node->state->val);
}

inline void AlphaZero::ai::Agent::fit(std::shared_ptr<Memory> memory, unsigned short run)
{
	for (int idx = 0; idx < Training_loops ; idx++) {
		auto batch = Model::getBatch(memory, Training_batch);
		this->model->fit(batch, run, idx);
	}
}

inline std::pair<float, std::vector<float>> AlphaZero::ai::Agent::predict(std::shared_ptr<Game::GameState> state)
{
	auto preds = this->model->predict(state);
	float& val = preds.first;
	std::vector<float>& polys = std::vector<float>(action_count);

	for (unsigned short idx = 0; idx < action_count; idx++) {
		polys[idx] = preds.second[0][idx].item<float>();
	}
	return { val, polys };
}

inline std::pair<int, std::vector<float>> AlphaZero::ai::Agent::derministicAction(std::shared_ptr<Node> const& node)
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
		sum += iter.second->N;
	}
	float sumF = (float)sum;
	for (auto& val : probs) {
		val = val / sumF;
	}
	return { action, probs };
}

inline std::pair<int, std::vector<float>> AlphaZero::ai::Agent::prabilisticAction(std::shared_ptr<Node> const& node)
{
	int action = -1;
	int idx = 0;
	float action_probs = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	unsigned int sum = 0;
	std::vector<float> probs = jce::vector::gen(action_count, 0.0f);

	for (auto const& iter : node->edges) {
		probs[iter.second->action] = ((float)iter.second->N);
		sum += iter.second->N;
	}
	float sumF = (float)sum;
	for (auto& val : probs) {
		val = val / sumF;
		action_probs -= val;
		if (action_probs < 0) {
			action = idx;
			break;
		}
		idx++;
	}
	return { action, probs };
}
