#pragma once

#include <config.hpp>
#include <sockpp/tcp_connector.h>
#include <ai/agent.hpp>


#define ELO_PORT 2551
#define ELO_IP "127.0.0.1"

namespace AlphaZero
{
	namespace elo
	{
		class eloClient
		{
		public: int send(int agent1, int agent2, int win) const;
		public: int setElo(int agent1, int eloRating) const;
		public: int getElo(int agent1) const;
		public: int getAgentWithClosestElo(int eloVal) const;
		};
	}
}

inline int AlphaZero::elo::eloClient::send(int agent1, int agent2, int win) const
{
	sockpp::socket_initializer sockInit;
	in_port_t port = ELO_PORT;
	std::string host = ELO_IP;
	sockpp::tcp_connector conn({ host, port });
	if (!conn)
	{
		std::cout << (conn.last_error_str()) << std::endl; 
		return -1;
	}

	int data[4] = { 3, agent1, agent2, win };
	conn.write(data, sizeof(data));

	int elo [1];
	conn.read(elo, sizeof(elo));
	return elo[0];
}

inline int AlphaZero::elo::eloClient::setElo(int agent1, int elo) const
{
	sockpp::socket_initializer sockInit;
	in_port_t port = ELO_PORT;
	std::string host = ELO_IP;
	sockpp::tcp_connector conn({ host, port });
	if (!conn)
	{
		std::cout << (conn.last_error_str()) << std::endl;
		return -1;
	}

	int data[3] = { 2, agent1, elo };
	conn.write(data, sizeof(data));

	int delo[1];
	conn.read(delo, sizeof(delo));
	return delo[0];
}

inline int AlphaZero::elo::eloClient::getElo(int agent1) const
{
	sockpp::socket_initializer sockInit;
	in_port_t port = ELO_PORT;
	std::string host = ELO_IP;
	sockpp::tcp_connector conn({ host, port });
	if (!conn)
	{
		std::cout << (conn.last_error_str()) << std::endl;
		return -1;
	}

	int data[2] = { 1, agent1 };
	conn.write(data, sizeof(data));

	int elo[1];
	conn.read(elo, sizeof(elo));
	return elo[0];
}

inline int AlphaZero::elo::eloClient::getAgentWithClosestElo(int val) const
{
	sockpp::socket_initializer sockInit;
	in_port_t port = ELO_PORT;
	std::string host = ELO_IP;
	sockpp::tcp_connector conn({ host, port });
	if (!conn)
	{
		std::cout << (conn.last_error_str()) << std::endl;
		return -1;
	}

	int data[2] = { -1, val };
	conn.write(data, sizeof(data));

	int elo[1];
	conn.read(elo, sizeof(elo));
	return elo[0];
}