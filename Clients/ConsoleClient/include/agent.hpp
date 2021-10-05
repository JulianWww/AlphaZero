#pragma once
#pragma comment( lib, "ws2_32.lib")

#include "game.hpp"
#include "modifications.hpp"
#include <memory>
#include <string>
#include <sockpp/tcp_connector.h>

namespace Agents
{
	class Agent
	{
	public: virtual int getAction(std::shared_ptr<AlphaZero::Game::Game> game) = 0;
	};
	class User : public Agent
	{
	public: virtual int getAction(std::shared_ptr<AlphaZero::Game::Game> game);
	private: int subGetAction(std::shared_ptr<AlphaZero::Game::Game> game);
	};

	class RemoteAgent : public Agent
	{
	private: sockpp::socket_initializer sockInit;
	private: std::string ip;
	private: in_port_t port;

	public: RemoteAgent(std::string host, in_port_t port);
	public: virtual int getAction(std::shared_ptr<AlphaZero::Game::Game> game);

	public: void toArr(int* arr, std::shared_ptr<AlphaZero::Game::Game> game);
	};
}

inline int Agents::User::getAction(std::shared_ptr<AlphaZero::Game::Game> game)
{
	game->render();
	modifications::bottomLable();
	return this->subGetAction(game);
}

inline char currentPlayerIcon(int player)
{
	switch(player)
	{
	case(1): {return 'X'; };
	case(-1): {return 'O'; };
	default: {return 'E'; }
	}
}

inline int Agents::User::subGetAction(std::shared_ptr<AlphaZero::Game::Game> game)
{
	std::cout << std::endl << "Move for " << currentPlayerIcon(game->state->player) << ": ";
	int res;
	std::cin >> res;
	for (auto const& val : game->state->allowedActions)
	{
		if (res == modifications::allowedActionModification(val))
		{
			return val;
		}
	}
	std::cout << std::endl << "\33[31;1mIllegal Move try again\33[0m" << std::endl;
	return this->subGetAction(game);
}

inline Agents::RemoteAgent::RemoteAgent(std::string _host, in_port_t port)
{
	this->ip = _host;
	this->port = port;
	this->sockInit = sockpp::socket_initializer();
}

inline int Agents::RemoteAgent::getAction(std::shared_ptr<AlphaZero::Game::Game> game)
{
	int arr[GameBoardHolderSize + 1];
	this->toArr(arr, game);

	sockpp::tcp_connector con({ this->ip, this->port });
	con.write(arr, (GameBoardHolderSize + 1) * sizeof(int));
	
	int out[1];
	con.read_n(out, sizeof(int));
	return out[0];
}

inline void Agents::RemoteAgent::toArr(int* arr, std::shared_ptr<AlphaZero::Game::Game> game)
{
	for (int idx = 0; idx < GameBoardHolderSize; idx++)
	{
		arr[idx] = game->state->gameBoard.test(idx);
	}
	arr[GameBoardHolderSize] = game->state->player;
}