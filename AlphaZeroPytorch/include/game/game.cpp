#include "game.hpp"

#define columOfset 7

AlphaZero::Game::GameState::GameState(IDType board, int _player) 
{
	this->initialize(board, _player);
}

AlphaZero::Game::GameState::GameState()
{
	this->initialize(IDType(), 1);
}

void AlphaZero::Game::GameState::initialize(IDType board, int _player)
{
	this->gameBoard = board;
	this->player = _player;
	this->allowedActions = this->getAllowedActions();
	this->gameIsDone();
}

std::shared_ptr<AlphaZero::Game::GameState> AlphaZero::Game::GameState::takeAction(int action)
{
	IDType newBoard = this->gameBoard;
	GameState::IdIndex(action, this->player, newBoard);

	std::shared_ptr<GameState> newState = std::make_shared<GameState>(newBoard, -this->player);
	return newState;
}

void AlphaZero::Game::GameState::gameIsDone()
{
	std::vector<std::vector<int>> winOptions = {
		/*
		+----+----+----+----+----+----+----+
		| 0  | 1  | 2  | 3  | 4  | 5  | 6  |
		+----+----+----+----+----+----+----+
		| 7  | 8  | 9  | 10 | 11 | 12 | 13 |
		+----+----+----+----+----+----+----+
		| 14 | 15 | 16 | 17 | 18 | 19 | 20 |
		+----+----+----+----+----+----+----+
		| 21 | 22 | 23 | 24 | 25 | 26 | 27 |
		+----+----+----+----+----+----+----+
		| 28 | 29 | 30 | 31 | 32 | 33 | 34 |
		+----+----+----+----+----+----+----+
		| 35 | 36 | 37 | 38 | 39 | 40 | 41 |
		+----+----+----+----+----+----+----+
		*/
		//horizontal
		{0,1,2,3},
		{1,2,3,4},
		{2,3,4,5},
		{3,4,5,6},

		{7,8,9,10},
		{8,9,10,11},
		{9,10,11,12},
		{10,11,12,13},

		{14,15,16,17},
		{15,16,17,18},
		{16,17,18,19},
		{17,18,19,20},

		{21,22,23,24},
		{22,23,24,25},
		{23,24,25,26},
		{24,25,26,27},

		{28,29,30,31},
		{29,30,31,32},
		{30,31,32,33},
		{31,32,33,34},

		{35,36,37,38},
		{36,37,38,39},
		{37,38,39,40},
		{38,39,40,41},
		//vertical
		{0 , 7,14,21},
		{7 ,14,21,28},
		{14,21,28,35},

		{1 ,8 ,15,22},
		{8 ,15,22,29},
		{15,22,29,36},

		{2,9,16,23},
		{9,16,23,30},
		{16,23,30,37},

		{3 ,10,17,24},
		{10,17,24,31},
		{17,24,31,38},

		{4 ,11,18,25},
		{11,18,25,32},
		{18,25,32,39},

		{5 ,12,19,26},
		{12,19,26,33},
		{19,26,33,40},

		{6 ,13,20,27},
		{13,20,27,34},
		{20,27,34,41},

		//diagonal topleft-bottomRight
		{14,22,30,38},

		{7, 15,23,31},
		{15,23,31,39},

		{0, 8, 16,24},
		{8, 16,24,32},
		{16,24,32,40},

		{1, 9 ,17,25},
		{9, 17,25,33},
		{17,25,33,41},

		{2, 10,18,26},
		{10,18,26,34},
		
		{3, 11,19,27},

		//diagonal topright-bottomleft
		{3, 9, 15,21},

		{4, 10,16,22},
		{10,16,22,28},

		{5, 11,17,23},
		{11,17,23,29},
		{17,23,29,35},

		{6, 12,18,24},
		{12,18,24,30},
		{18,24,30,36},

		{13,19,25,31},
		{19,25,31,37},

		{20,26,32,38},
	};
	bool tie = true;
	for (int idx = 0; idx < action_count; idx++) {
		if (this->IdIndex(idx) == 0) {
			tie = false;
			break;
		}
	}
	if (tie) {
		this->done = true;
		this->val = { 0,0,0 };
		return;
	}
	for (auto option : winOptions) {
		int count = 0;
		for (int pos : option) {
			count += this->IdIndex(pos);
		}
		if (count == -4 * this->player) {
			this->done = true;
			this->val = { -1, -1, 1 }; // winForThisPlayer, points for this player, points for other player
			return;
		}
	}
	this->done = false;
	this->val = { 0, 0, 0 };
}

inline std::pair<int, bool> AlphaZero::Game::GameState::getAllowedColumHeight(int idx) {
	if (this->IdIndex(idx) != 0) {
		return { idx, false };
	}
	if (idx >= 35) {
		return {idx, true};
	}
	else if (this->IdIndex(idx + columOfset)!=0) {
		return { idx, true };
	}
	else {
		return this->getAllowedColumHeight(idx + columOfset);
	}
}

std::list<int> AlphaZero::Game::GameState::getAllowedActions()
{
	std::list<int> res;
	for (int idx = 0; idx < 7; idx++) {
		std::pair<int, bool> data = this->getAllowedColumHeight(idx);
		if (data.second) {
			res.push_back(data.first);
		}
	}
	return res;
}

void AlphaZero::Game::GameState::render()
{
	for (int row = 0; row < action_count;) {
		for (int iter = 0; iter < 7; iter++) {
			std::cout << this->getPiece(row) << " ";
			row++;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

#if (MainLogger || MCTSLogger || MemoryLogger || ProfileLogger || ModelLogger)
void AlphaZero::Game::GameState::render(std::shared_ptr<spdlog::logger> logger)
{
	for (int idx = 0; idx < 6; idx++) {
		char line1[13] = {
			this->getPiece(0 + columOfset * idx), ' ',
			this->getPiece(1 + columOfset * idx), ' ',
			this->getPiece(2 + columOfset * idx), ' ',
			this->getPiece(3 + columOfset * idx), ' ',
			this->getPiece(4 + columOfset * idx), ' ',
			this->getPiece(5 + columOfset * idx), ' ',
			this->getPiece(6 + columOfset * idx)
		};
		logger->info(line1);
	}
}
#endif

AlphaZero::Game::Game::Game()
{
	this->state = std::make_shared<GameState>();
}

void AlphaZero::Game::Game::reset()
{
	this->state = std::make_shared<GameState>();
}

void AlphaZero::Game::Game::takeAction(int action)
{
	auto newState = this->state->takeAction(action);
	this->state = newState;
}

bool AlphaZero::Game::Game::takeHumanAction(int action)
{
	for (auto const& allowed : this->state->allowedActions) {
		if ((allowed - action) % 7 == 0) {
			this->takeAction(allowed);
			return true;
		}
	}
	return false;
}

inline std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<float>> mirrorGameState(std::shared_ptr<AlphaZero::Game::GameState> state, std::vector<float>& actionProbs) {
	IDType boardBuffer;

	std::vector<float> probs = {
		actionProbs[6],  actionProbs[5],  actionProbs[4],  actionProbs[3],  actionProbs[2],  actionProbs[1],  actionProbs[0],
		actionProbs[13], actionProbs[12], actionProbs[11], actionProbs[10], actionProbs[9],  actionProbs[8],  actionProbs[7],
		actionProbs[20], actionProbs[19], actionProbs[18], actionProbs[17], actionProbs[16], actionProbs[15], actionProbs[14],
		actionProbs[27], actionProbs[26], actionProbs[25], actionProbs[24], actionProbs[23], actionProbs[22], actionProbs[21],
		actionProbs[34], actionProbs[33], actionProbs[32], actionProbs[31], actionProbs[30], actionProbs[29], actionProbs[28],
		actionProbs[41], actionProbs[40], actionProbs[39], actionProbs[38], actionProbs[37], actionProbs[36], actionProbs[35]
	};
#define assignStateSinge(idx1, idx2)AlphaZero::Game::GameState::IdIndex(idx1, state->IdIndex(idx2), boardBuffer)
#define assignState(idx1, idx2)assignStateSinge(idx1, idx2);assignStateSinge(idx2, idx1);


	assignState(0,  6);  assignState(1,  5);  assignState(2,  4);  assignStateSinge(3,  3);
	assignState(7,  13); assignState(8,  12); assignState(9,  11); assignStateSinge(10, 10);
	assignState(14, 20); assignState(15, 19); assignState(16, 18); assignStateSinge(17, 17);
	assignState(21, 27); assignState(22, 26); assignState(23, 25); assignStateSinge(24, 24);
	assignState(28, 34); assignState(29, 33); assignState(30, 32); assignStateSinge(31, 31);
	assignState(35, 41); assignState(36, 40); assignState(37, 39); assignStateSinge(38, 38);
#undef assignState

	return { std::make_shared<AlphaZero::Game::GameState>(boardBuffer, state->player), probs };
}

std::vector<std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<float>>> AlphaZero::Game::identities(std::shared_ptr<GameState> state, std::vector<float>& probs)
{
	std::vector<std::pair<std::shared_ptr<GameState>, std::vector<float>>> idents(2);
	int id = 0;
	idents[0] = { state, probs };
	idents[1] = mirrorGameState(state, probs);
	return idents;
}