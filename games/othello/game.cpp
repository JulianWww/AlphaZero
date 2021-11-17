#include "game.hpp"

#define columOfset 8

std::vector<std::pair<int, int>> ajacentDirections = { {1,0},{-1,0},{0,1},{0,-1} };
std::vector<std::pair<int, int>> flipDirections = { {1,0},{-1,0},{0,1}, {0,-1},	{1,1},{1,-1},{-1,-1},{-1,1} };

bool isValidPostition(int const& x, int const& y)
{
	if (x < 0) { return false; }
	if (y < 0) { return false; }
	if (x >= input_shape_x) { return false; }
	if (y >= input_shape_y) { return false; }
	return true;
}

std::vector<int> AlphaZero::Game::GameState::getFlipActions(int x, int y, int dx, int dy)
{
	std::vector<int> tmp;
	std::pair<int, int> a = { x + dx, y + dy };
	auto value = this->player;
	while (isValidPostition(a.first, a.second))
	{
		auto idx = from2dPos(a.first, a.second);
		auto otherValue = this->IdIndex(idx);
		if (value == -otherValue)
		{
			tmp.push_back(idx);
		}
		else if (value == otherValue)
		{
			return tmp;
		}
		else
		{
			return std::vector<int>();
		}
		a = { a.first + dx, a.second + dy };
	}
	return std::vector<int>();
}

bool AlphaZero::Game::GameState::hasAjacentStones(int const& x, int const& y)
{
	int otherX, otherY;
	for (auto const& direction : ajacentDirections)
	{
		otherX = x + direction.first;
		otherY = y + direction.second;
		if (isValidPostition(otherX, otherY))
		{
			if (this->IdIndex(from2dPos(otherX, otherY)) != 0)
			{
				return true;
			}
		}
	}
	return false;
}

AlphaZero::Game::GameState::GameState(IDType board, int _player) 
{
	this->initialize(board, _player);
}

AlphaZero::Game::GameState::GameState()
{
	IDType board;
	this->IdIndex(27, -1, board);
	this->IdIndex(28,  1, board);
	this->IdIndex(35,  1, board);
	this->IdIndex(36, -1, board);
	this->initialize(board, 1);
}

void AlphaZero::Game::GameState::initialize(IDType board, int _player)
{
	this->gameBoard = board;
	this->player = _player;
	this->getAllowedActions();
	this->gameIsDone();
}

std::shared_ptr<AlphaZero::Game::GameState> AlphaZero::Game::GameState::takeAction(int action)
{
	IDType newBoard = this->gameBoard;
	GameState::IdIndex(action, this->player, newBoard);
	std::pair<int, int> pos = to2dPos(action);
	for (auto const& direction : flipDirections)
	{
		auto toFlip = getFlipActions(pos.first, pos.second, direction.first, direction.second);
		for (auto const& pos : toFlip)
		{
			GameState::IdIndex(pos, this->player, newBoard);
		}
	}

	std::shared_ptr<GameState> newState = std::make_shared<GameState>(newBoard, -this->player);

	if (newState->allowedActions.size())
		return newState;

	std::shared_ptr<GameState> newerState = std::make_shared<GameState>(newBoard, this->player);
	return newerState;
}

void AlphaZero::Game::GameState::gameIsDone()
{
	int thisPlayerPoints = 0, otherPlayerPoints = 0;
	this->done = true;
	for (int x = 0; x < input_shape_x; x++)
	{
		for (int y = 0; y < input_shape_y; y++)
		{
			auto val = this->IdIndex(x + y * columOfset);
			if (val == 0 && this->allowedActions.size())
			{
				this->done = false;
			}
			else if (val == this->player)
			{
				thisPlayerPoints++;
			}
			else if (val == -this->player)
			{
				otherPlayerPoints++;
			}
		}
	}
	if (this->done)
	{
		int win = 0;
		if (thisPlayerPoints > otherPlayerPoints)
		{
			win = 1;
		}
		else if (thisPlayerPoints < otherPlayerPoints)
		{
			win = -1;
		}
		this->val = { win, thisPlayerPoints, otherPlayerPoints };
	}
	else
	{
		this->val = { 0, 0, 0 };
	}
}

void AlphaZero::Game::GameState::getAllowedActions()
{
	this->allowedActions.clear();
	for (int x = 0; x < input_shape_x; x++)
	{
		for (int y = 0; y < input_shape_y; y++)
		{
			if (this->IdIndex(from2dPos(x, y)) == 0 && hasAjacentStones(x, y))
			{
				for (auto const& direction : flipDirections)
				{
					auto toFlip = getFlipActions(x, y, direction.first, direction.second);
					if (toFlip.size())
					{
						this->allowedActions.push_back(from2dPos(x, y));
						break;
					}
				}
			}
		}
	}
}

void AlphaZero::Game::GameState::render()
{
	std::vector<GameState*> state = { this };
	renderStates(state);
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
	this->takeAction(action);
	return true;
}

#define assign(idx1, idx2)AlphaZero::Game::GameState::IdIndex(idx1, state->IdIndex(idx2), boardBuffer)
#define assignState(idx1, idx2)assign(idx1, idx2);assign(idx2, idx1)

inline std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<int>> mirrorGameState(std::shared_ptr<AlphaZero::Game::GameState> state, std::vector<int> const& actionProbs) {
	IDType boardBuffer;

	std::vector<int> probs = {
		actionProbs[7],  actionProbs[6],  actionProbs[5],  actionProbs[4],  actionProbs[3],  actionProbs[2],  actionProbs[1],  actionProbs[0],
		actionProbs[15], actionProbs[14], actionProbs[13], actionProbs[12], actionProbs[11], actionProbs[10], actionProbs[9],  actionProbs[8],
		actionProbs[23], actionProbs[22], actionProbs[21], actionProbs[20], actionProbs[19], actionProbs[18], actionProbs[17], actionProbs[16],
		actionProbs[31], actionProbs[30], actionProbs[29], actionProbs[28], actionProbs[27], actionProbs[26], actionProbs[25], actionProbs[24],
		actionProbs[39], actionProbs[38], actionProbs[37], actionProbs[36], actionProbs[35], actionProbs[34], actionProbs[33], actionProbs[32],
		actionProbs[47], actionProbs[46], actionProbs[45], actionProbs[44], actionProbs[43], actionProbs[42], actionProbs[41], actionProbs[40],
		actionProbs[55], actionProbs[54], actionProbs[53], actionProbs[52], actionProbs[51], actionProbs[50], actionProbs[49], actionProbs[48],
		actionProbs[63], actionProbs[62], actionProbs[61], actionProbs[60], actionProbs[59], actionProbs[58], actionProbs[57], actionProbs[56]
	};


	assignState(0, 7);   assignState(1, 6);   assignState(2, 5);   assignState(3, 4);
	assignState(8, 15);  assignState(9, 14);  assignState(10, 13); assignState(11, 12);
	assignState(16, 23); assignState(17, 22); assignState(18, 21); assignState(19, 20);
	assignState(24, 31); assignState(25, 30); assignState(26, 29); assignState(27, 28);
	assignState(32, 39); assignState(33, 38); assignState(34, 37); assignState(35, 36);
	assignState(40, 47); assignState(41, 46); assignState(42, 45); assignState(43, 44);
	assignState(48, 55); assignState(49, 54); assignState(50, 53); assignState(51, 52);
	assignState(56, 63); assignState(57, 62); assignState(58, 61); assignState(59, 60);

	return { std::make_shared<AlphaZero::Game::GameState>(boardBuffer, state->player), probs };
}

inline std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<int>> rotateGameState(std::shared_ptr<AlphaZero::Game::GameState> state, std::vector<int> const& actionProbs)
{
	IDType boardBuffer;

	std::vector<int> probs = {
		actionProbs[56], actionProbs[48], actionProbs[40], actionProbs[32], actionProbs[24], actionProbs[16], actionProbs[8],  actionProbs[0],
		actionProbs[57], actionProbs[49], actionProbs[41], actionProbs[33], actionProbs[25], actionProbs[17], actionProbs[9],  actionProbs[1],
		actionProbs[58], actionProbs[50], actionProbs[42], actionProbs[34], actionProbs[26], actionProbs[18], actionProbs[10], actionProbs[2],
		actionProbs[59], actionProbs[51], actionProbs[43], actionProbs[35], actionProbs[27], actionProbs[19], actionProbs[11], actionProbs[3],
		actionProbs[60], actionProbs[52], actionProbs[44], actionProbs[36], actionProbs[28], actionProbs[20], actionProbs[12], actionProbs[4],
		actionProbs[61], actionProbs[53], actionProbs[45], actionProbs[37], actionProbs[29], actionProbs[21], actionProbs[13], actionProbs[5],
		actionProbs[62], actionProbs[54], actionProbs[46], actionProbs[38], actionProbs[30], actionProbs[22], actionProbs[14], actionProbs[6],
		actionProbs[63], actionProbs[55], actionProbs[47], actionProbs[39], actionProbs[31], actionProbs[23], actionProbs[15], actionProbs[7]
	};

	auto a = state->IdIndex(0);
	assign(0, 56);  assign(1, 48);  assign(2, 40);  assign(3, 32);  assign(4, 24);  assign(5, 16);  assign(6, 8);   assign(7, 0);
	assign(8, 57);  assign(9, 49);  assign(10, 41); assign(11, 33); assign(12, 25); assign(13, 17); assign(14, 9);  assign(15, 1);
	assign(16, 58); assign(17, 50); assign(18, 42); assign(19, 34); assign(20, 26); assign(21, 18); assign(22, 10); assign(23, 2);
	assign(24, 59); assign(25, 51); assign(26, 43); assign(27, 35); assign(28, 27); assign(29, 19); assign(30, 11); assign(31, 3);
	assign(32, 60); assign(33, 52); assign(34, 44); assign(35, 36); assign(36, 28); assign(37, 20); assign(38, 12); assign(39, 4);
	assign(40, 61); assign(41, 53); assign(42, 45); assign(43, 37); assign(44, 29); assign(45, 21); assign(46, 13); assign(47, 5);
	assign(48, 62); assign(49, 54); assign(50, 46); assign(51, 38); assign(52, 30); assign(53, 22); assign(54, 14); assign(55, 6);
	assign(56, 63); assign(57, 55); assign(58, 47); assign(59, 39); assign(60, 31); assign(61, 23); assign(62, 15); assign(63, 7);

	return { std::make_shared<AlphaZero::Game::GameState>(boardBuffer, state->player), probs };
}

#undef assignState
#undef assign

bool canBeAddedToIdentities(std::vector<std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<int>>>const& idents, std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<int>>const& data)
{
	auto pos = std::find(idents.begin(), idents.end(), data);
	return pos == idents.end();
}

std::vector<std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<int>>> AlphaZero::Game::identities(std::shared_ptr<GameState> state, std::vector<int>& probs)
{
	std::vector<std::pair<std::shared_ptr<AlphaZero::Game::GameState>, std::vector<int>>> idents;
	int idx = 0;
	std::pair<std::shared_ptr<GameState>, std::vector<int>> data = { state, probs };
	std::pair<std::shared_ptr<GameState>, std::vector<int>> mirrored = mirrorGameState(state, probs);
	for (unsigned short iter = 0; iter < 3; iter++)
	{
		if (canBeAddedToIdentities(idents, data))
			idents.push_back(data);
		if (canBeAddedToIdentities(idents, mirrored))
			idents.push_back(mirrored);

		data = rotateGameState(data.first, data.second);
		mirrored = rotateGameState(mirrored.first, mirrored.second);
	}
	if (canBeAddedToIdentities(idents, data))
		idents.push_back(data);
	if (canBeAddedToIdentities(idents, mirrored))
		idents.push_back(mirrored);
	return idents;
}

void AlphaZero::Game::renderStates(std::vector<GameState*> states)
{
	console_mutex.lock();
	for (int y = 0; y < input_shape_y; y++)
	{
		for (auto const& state : states)
		{
			for (int x = 0; x < input_shape_x; x++)
			{
				std::cout << state->getPiece(from2dPos(x, y)) << " ";
			}
			std::cout << "\t\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	console_mutex.unlock();
}