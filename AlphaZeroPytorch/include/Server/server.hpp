#pragma once

#include <config.hpp>
#include <sockpp/tcp_acceptor.h>
#include <ai/agent.hpp>


#define PORT 25500

namespace AlphaZero
{
	namespace Server
	{
		IDType toBoard(int arr[]);

		class TCPServer
		{
		private: void evaluate(sockpp::tcp_socket& sock);
		private: sockpp::socket_initializer sockInit;
		private: sockpp::tcp_acceptor acc;
		private: void accept();

		public: TCPServer(int port = PORT);
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