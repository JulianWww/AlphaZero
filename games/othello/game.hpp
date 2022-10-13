#pragma once
/*
see the game file in AlphaZeroPytorch/include/game but modified for Orthello, the functions are self-explanatory
*/

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <bitset>
#include <unordered_set>
#include <torch/torch.h>

#include "config.hpp"


#define input_shape_x 8
#define input_shape_y 8
#define input_snape_z 2
#define action_count 64
#define action_shape 8, 8
#define boardOfset 64 // the size of a layer of the board in the buffer. (the amount of felds)
#define gameName "Orthello"


namespace AlphaZero {
	namespace Game {
		class GameState {
		public: int player;
		public: bool done;
		public: std::tuple<int, int, int> val;
		public: IDType gameBoard;
		public: std::vector<int> allowedActions;

		public: GameState(IDType board, int _player);
		public: GameState();
		private: void initialize(IDType board, int _player);
		public: std::shared_ptr<GameState> takeAction(int action);
		public: void gameIsDone();
		protected: void getAllowedActions();
		public: int IdIndex(int id);
		public: IDType id();
		public: void render();
#if (MainLogger || MCTSLogger || MemoryLogger || ProfileLogger || ModelLogger)
		public: void render(std::shared_ptr<spdlog::logger> logger);
#endif
		public: void static IdIndex(int id, int val, IDType& b);
		public: torch::Tensor toTensor();
		public: void toTensor(torch::Tensor& tensor, unsigned short idx=0);
		public: char getPiece(int val);
		private: std::vector<int> getFlipActions(int x, int y, int dx, int dy);
		private: bool hasAjacentStones(int const& x, int const& y);
		};
		struct StateHash
		{
			std::size_t operator()(std::pair<std::shared_ptr<GameState>, std::vector<int>> const& s) const noexcept;
		};
		// optimization function its not a problem if not all are found
		std::vector<std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<int>>> identities(std::shared_ptr<GameState> state, std::vector<int>& actionProbs);

		class Game {
		public: std::tuple<int, int> BoardShape = { 3,3 };
		public: std::tuple<int, int, int> inputShape = { 2,3,3 };
		public: std::shared_ptr<GameState> state;

		public: Game();
		public: void reset();
		public: void takeAction(int action);
		public: bool takeHumanAction(int action);
		public: void render();
		};

		inline void test() {
			AlphaZero::Game::Game game;

			while (!game.state->done) {
				auto idx = std::rand() % game.state->allowedActions.size();
				auto action = game.state->allowedActions[idx];
				game.render();
				game.takeAction(action);
				char arr[100];

				//system("cls");
			}
			game.render();

			std::cout << std::endl << "the last player just won";
		}
		std::pair<int, int> to2dPos(int pos);
		int from2dPos(int const& x, int const& y);

		void renderStates(std::vector<GameState*> states);
	}
}

inline std::size_t AlphaZero::Game::StateHash::operator()(std::pair<std::shared_ptr<GameState>, std::vector<int>>  const& s) const noexcept {
	return s.first->gameBoard.to_ullong();
}

inline int AlphaZero::Game::GameState::IdIndex(int id)
{
	if (this->gameBoard[id] == 1) {
		return 1;
	}
	else if (this->gameBoard[id + boardOfset] == 1) {
		return -1;
	}
	return 0;
}

inline void AlphaZero::Game::GameState::IdIndex(int id, int val, IDType& b)
{
	int otherId;
	if (val == 0) {
		b.set(id, 0);
		b.set(id + boardOfset, 0);
		return;
	}
	if (val == 1)
	{
		otherId = id + boardOfset;
	}
	if (val ==-1) {
		otherId = id;
		id += boardOfset;
	}
	b.set(id, 1);
	b.set(otherId, 0);
}

inline char AlphaZero::Game::GameState::getPiece(int id)
{
	std::unordered_map<int, char> renderData = { {0,'-'}, {1, 'X'}, {-1, 'O'} };
	if (std::find(this->allowedActions.begin(), this->allowedActions.end(), id) != this->allowedActions.end()) {
		return '+';
	}
	auto va = renderData[this->IdIndex(id)];
	return va;
}

inline IDType AlphaZero::Game::GameState::id()
{
	return this->gameBoard;
}

inline void AlphaZero::Game::Game::render() {
	this->state->render();
}

inline torch::Tensor AlphaZero::Game::GameState::toTensor() 
{
	at::Tensor outTensor = at::zeros({ 1, input_snape_z, input_shape_y, input_shape_x });
	this->toTensor(outTensor);
	return outTensor;
}

inline void AlphaZero::Game::GameState::toTensor(torch::Tensor& tensor, unsigned short idx)
{
	unsigned short pos = 0;
	unsigned int ofset = (this->player == -1) ? 0 : boardOfset;
	for (unsigned short z = 0; z < input_snape_z; z++) {
		for (unsigned short y = 0; y < input_shape_y; y++) {
			for (unsigned short x = 0; x < input_shape_x; x++) {
				tensor[idx][z][y][x] = (float)this->gameBoard[(pos + ofset) % stateSize];
				pos++;
			}
		}
	}
}

inline std::pair<int, int> AlphaZero::Game::to2dPos(int pos)
{
	int x = pos % input_shape_x;
	int y = (pos - x) / input_shape_x;
	return { x,y };
}

inline int AlphaZero::Game::from2dPos(int const& x, int const& y)
{
	return x + y * input_shape_x;
}