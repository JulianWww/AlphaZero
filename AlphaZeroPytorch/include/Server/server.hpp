#pragma once

#include <config.hpp>
#include <sockpp/tcp_acceptor.h>
#include <ai/agent.hpp>


#define PORT 12345

namespace AlphaZero 
{
	namespace Server
	{
		IDType toBoard(int arr[]);

		class TCPServer
		{
		private: sockpp::socket_initializer sockInit;
		private: sockpp::tcp_acceptor acc;
		private: std::shared_ptr<ai::Agent> agent;
		private: void accept();

		public: TCPServer(std::shared_ptr<ai::Agent> agent, int port=PORT);
		public: void mainLoop();
		};

		class TestServer
		{
		private: sockpp::socket_initializer sockInit;
		private: sockpp::tcp_acceptor acc;
		private: void accept();
			   
		public: TestServer(int port = PORT);
		public: void mainLoop();
		};
	}
}

inline IDType AlphaZero::Server::toBoard(int arr[])
{
	IDType out;
	for (int idx = 0; idx < stateSize; idx++)
	{
		out.set(idx, arr[idx]);
	}
	return out;
}