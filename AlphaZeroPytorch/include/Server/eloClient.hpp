#pragma once

#include <config.hpp>
#include <sockpp/tcp_connector.h>
#include <ai/agent.hpp>


#define ELO_PORT 2551
#define ELO_IP "localhost"

namespace AlphaZero
{
	namespace elo
	{
    // class used to communicate with the python elo data server,
		class eloClient
		{
      // update the elo rating using the traditional method
		public: int send(int agent1, int agent2, int win) const;
      // set the elo rating of an agent
		public: int setElo(int agent1, int eloRating) const;
      // get the elo rating of a certian agent
		public: int getElo(int agent1) const;
      // get the agent with the closest elo
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