#pragma once
#include <ai/model.hpp>
#include <ai/memory.hpp>
#include <jce/vector.hpp>
#include "utils.hpp"
#include <thread>
#include "MCTS.hpp"

namespace AlphaZero {
	namespace ai {
		class Agent {
      // hashmap of thread ids to MCTS trees

		public: std::unordered_map<size_t, std::shared_ptr<MCTS>> tree;

      // Neural network pointer
		public: std::unique_ptr<AlphaZero::ai::Model> model;
		public: Agent(const char* device);

      // uuid of the agent for hashing purposes
		public: int identity;

      // get the MCTS tree for a certain thread
		public: MCTS* getTree();

      // clear MCTS tree chach
		public: void reset();

      // get the action, the agent selected
#if Training
		public: std::pair<int, std::pair<std::vector<int>, float>> getAction(std::shared_ptr<Game::GameState> state, bool proabilistic);
#else
		public: virtual std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::GameState> state, bool proabilistic);
#endif

      // run MCTS simulations
		public: void runSimulations(Node*, MCTS* tree);

      // evaluate a leaf inside the MCTS
		private: float evaluateLeaf(Node*, MCTS* tree);

      // fit the neural network to data provided by the memory
		public: void fit(std::shared_ptr<Memory> memory, unsigned short iteration);

      // predict value and policy for a certain game state using the NN
		public: std::pair<float, std::vector<float>> predict(std::shared_ptr<Game::GameState> state);

      // select an action deteministicly
		private: std::pair<int, std::pair<std::vector<int>, float>> derministicAction(Node* node);

      // select an action proabilisticly
		private: std::pair<int, std::pair<std::vector<int>, float>> prabilisticAction(Node* node);
		};
#if not Training
		class User : public Agent {
		public: virtual std::pair<int, std::vector<float>> getAction(std::shared_ptr<Game::Game> game, bool proabilistic);
		};
#endif

      // run simluations bc threading stuff
		void runSimulationsCaller(AlphaZero::ai::Agent* agent, Node* node, MCTS* tree);
	}
}

inline AlphaZero::ai::MCTS* AlphaZero::ai::Agent::getTree()
{
	auto a = std::hash<std::thread::id>{}(std::this_thread::get_id());
	if (this->tree.count(a))
	{
		return this->tree[a].get();
	}
	else
	{
		auto tree = std::make_shared<MCTS>();
		this->tree.insert({ a, tree });
		return tree.get();
	}
}

inline void AlphaZero::ai::Agent::reset()
{
	this->tree.clear();
}

inline void AlphaZero::ai::Agent::runSimulations(Node* node, MCTS* tree)
{
	std::pair<Node*, std::list<Edge*>> serchResults = tree->moveToLeaf(node);
	float val = this->evaluateLeaf(serchResults.first, tree);
	tree->backFill(serchResults.second, serchResults.first, val);
	tree->addMCTSIter();
}

inline void AlphaZero::ai::runSimulationsCaller(AlphaZero::ai::Agent* agent, Node* node, MCTS* tree)
{
	while (tree->MCTSIter < MCTSSimulations) {
		agent->runSimulations(node, tree);
	}
}

inline float AlphaZero::ai::Agent::evaluateLeaf(Node* node, MCTS* tree)
{
	if (!node->state->done){
		std::shared_ptr<Game::GameState> nextState;
		Node* nextNode;
		auto data = this->predict(node->state);
		for (auto& action : node->state->allowedActions) {
			nextState = node->state->takeAction(action);
			nextNode = tree->addNode(nextState);
			Edge newEdge = Edge(nextNode, node, action, data.second[action]); //the last is the prob
			node->addEdge( action, newEdge);
		}
		return data.first;
	}
	return (float)std::get<0>(node->state->val);
}

inline void AlphaZero::ai::Agent::fit(std::shared_ptr<Memory> memory, unsigned short run)
{
	std::cout << "\33[35;1mretraining\33[0m" << std::endl;
	for (int idx = 0; idx < Training_loops ; idx++) {
#if RenderTrainingProgress
		jce::consoleUtils::render_progress_bar((float)idx / (float)Training_loops);
#endif
		auto batch = Model::getBatch(memory, Training_batch);
		for (size_t trainingEpoch = 0; trainingEpoch < Training_epochs; trainingEpoch++)
			this->model->fit(batch, run, idx);
		
#if LossLogger
		debug::log::_lossLogger.newBatch();
#endif
	}
#if LossLogger
	debug::log::_lossLogger.save("logs/games/loss.bin");
#endif
#if RenderTrainingProgress
	jce::consoleUtils::render_progress_bar(1.0f, true);
#endif
}

inline std::pair<float, std::vector<float>> AlphaZero::ai::Agent::predict(std::shared_ptr<Game::GameState> state)
{
	auto preds = this->model->predict(state);
	float& val = preds.first;
	std::vector<float> polys = std::vector<float>(action_count);

	c10::Device device ("cpu");

	torch::Tensor mask = torch::ones(
		{ 1, action_count }, 
		c10::TensorOptions().device(c10::Device("cpu")).dtype(at::kBool)
	);

	for (auto idx : state->allowedActions)
	{
		mask[0][idx] = false;
	}

	torch::Tensor out = torch::softmax(torch::masked_fill(preds.second.cpu(), mask, -1000.0f), 1);

	for (auto const& idx : state->allowedActions) {
		polys[idx] = out[0][idx].item<float>();
	}

	return { val, polys };
}

inline std::pair<int, std::pair<std::vector<int>, float>> AlphaZero::ai::Agent::derministicAction(Node* node)
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
	return { action, { probs, node->edges[action].Q } };
}

inline std::pair<int, std::pair<std::vector<int>, float>> AlphaZero::ai::Agent::prabilisticAction(Node* node)
{
	int action = -1;
	int idx = 0;
	unsigned int sum = 0;
	std::vector<int> probs = jce::vector::gen(action_count, 0);

	for (auto const& iter : node->edges) {
		probs[iter.second.action] = (iter.second.N);
	}
	auto action_probs = (rand() % ai::getSumm(probs));

	for (auto const& val : probs) {
		action_probs -= val;
		if (action_probs < 0) {
			action = idx;
			break;
		}
		idx++;
	}
	return { action, { probs, node->edges[action].Q } };
}
