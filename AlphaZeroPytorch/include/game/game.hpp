#pragma once
/*
this is the alpha Zero game for tick tack toe
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


#define input_shape_x 7
#define input_shape_y 6
#define input_snape_z 2
#define action_count 42
#define boardOfset 42 // the size of a layer of the board in the buffer. (the amount of felds)
#define gameName "connect4"


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
		private: char getPiece(int val);
		private: std::pair<int, bool> getAllowedColumHeight(int);
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
			AlphaZero::Game::Game* game = new AlphaZero::Game::Game();

			while (!game->state->done) {
				std::vector<int> vec = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 };
				auto idents = identities(game->state, vec);
				idents[1].first->render();
				std::cout << "your action: ";
				int action;
				std::cin >> action;
				game->takeHumanAction(action);
#if Windows
				system("cls");
#else
				system("clear");
#endif
			}
			game->render();

			std::cout << std::endl << "the last player just won";
		}
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
	if (val == 0) {
		b.set(id, 0);
		b.set(id + boardOfset, 0);
		return;
	}
	if (val == -1) {
		id += boardOfset;
	}
	b.set(id, 1);
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