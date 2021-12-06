#pragma once

#include <config.hpp>
#include <sockpp/tcp_connector.h>
#include <ai/agent.hpp>


#define ELO_PORT 2551

namespace AlphaZero
{
	namespace elo
	{
		class eloClient
		{
		public: int send(int agent1, int agent2, int win);
		public: int send(int agent1, int eloRating);
		};
	}
}

inline int AlphaZero::elo::eloClient::send(int agent1, int agent2, int win)
{
	sockpp::socket_initializer sockInit;
	in_port_t port = ELO_PORT;
	std::string host = "127.0.0.1";
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

inline int AlphaZero::elo::eloClient::send(int agent1, int elo)
{
	sockpp::socket_initializer sockInit;
	in_port_t port = ELO_PORT;
	std::string host = "127.0.0.1";
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