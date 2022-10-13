#pragma once

#include <config.hpp>
#include <sockpp/tcp_acceptor.h>
#include <ai/agent.hpp>


#define PORT 25500

namespace AlphaZero
{
	namespace Server
	{
      // convert int array to binary board array
		IDType toBoard(int arr[]);
      // tcp game server
		class TCPServer
		{
      // read data from sock, pass to Agent and send back recomendation
		private: void evaluate(sockpp::tcp_socket& sock);
		private: sockpp::socket_initializer sockInit;
      // server sock
		private: sockpp::tcp_acceptor acc;
      // accept connection from remote
		private: void accept();

      // create TCP sever on port
		public: TCPServer(int port = PORT);
      // run accept, evalute loop
		public: void mainLoop();
		};
      // Identical to the main server but prints out request and uses this console input as return, use for debug
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