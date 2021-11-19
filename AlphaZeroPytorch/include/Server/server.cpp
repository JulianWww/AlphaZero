// SockServer.cpp : Defines the entry point for the application.
//

#pragma comment( lib, "ws2_32.lib")

#include "server.hpp"
#include <game/game.hpp>
#include <iostream>
#include <log.hpp>

std::shared_ptr<spdlog::logger> logger = debug::log::createLogger("ServerLogger", "logs/c++/Server.log");


void AlphaZero::Server::TCPServer::accept()
{
	sockpp::tcp_socket sock = this->acc.accept();

	logger->info("Connection acceptd from ", sock.peer_address().to_string());

	ssize_t n;
	int buf[stateSize + 1];
	int out[1];

	n = sock.read(buf, sizeof(buf));

	std::shared_ptr<Game::GameState> state = std::make_shared<Game::GameState>(toBoard(buf), buf[stateSize]);

	auto actionData = this->agent->getAction(state, false);
	out[0] = actionData.first;

#if MainLogger
	state->render(logger);
	logger->info("MSCT vals: {:1.5f}", actionData.second.second);
	debug::log::logVector(logger, actionData.second.first);
	logger->info("NN vals: {:1.5f}", this->agent->predict(state).first);
	debug::log::logVector(logger, this->agent->predict(state).second);

	logger->info("selected action is: {}", actionData.first);
#endif

	sock.write_n(out, sizeof(int));

	logger->info("Connection closed");
}

AlphaZero::Server::TCPServer::TCPServer(std::shared_ptr<ai::Agent> _agent, int _port): agent(_agent)
{
	in_port_t port = _port;
	this->acc = sockpp::tcp_acceptor(port);

	if (!acc) {
		std::cerr << "Error creating the acceptor: " << acc.last_error_str() << std::endl;
	}

	std::cout << "Acceptor bound to address: " << this->acc.address() << std::endl;
	std::cout << "Awaiting connections on port " << port << "..." << std::endl;

	logger->info("Acceptor bound to address: ", this->acc.address().to_string());
	logger->info("Awaiting connections on port: {}", port);
}

void AlphaZero::Server::TCPServer::mainLoop()
{
	while (true)
	{
		this->accept();
	}
}

AlphaZero::Server::TestServer::TestServer(int _port)
{
	in_port_t port = _port;
	this->acc = sockpp::tcp_acceptor(port);

	if (!acc) {
		std::cerr << "Error creating the acceptor: " << acc.last_error_str() << std::endl;
	}
	std::cout << "Acceptor bound to address: " << acc.address() << std::endl;
	std::cout << "Awaiting connections on port " << port << "..." << std::endl;
}

void AlphaZero::Server::TestServer::mainLoop()
{
	while (true)
	{
		this->accept();
	}
}

void AlphaZero::Server::TestServer::accept()
{
	sockpp::tcp_socket sock = this->acc.accept();

	std::cout << "Connection acceptd from " << sock.peer_address() << std::endl;

	ssize_t n;
	int buf[stateSize + 1];
	int out[1];

	n = sock.read(buf, sizeof(buf));

	std::shared_ptr<Game::GameState> state = std::make_shared<Game::GameState>(toBoard(buf), buf[stateSize]);
	state->render();
	
	std::cout << "Server Action for testing: ";
	std::cin >> out[0];
	std::cout << std::endl;

	sock.write_n(out, sizeof(int));

	std::cout << "Connection closed from " << sock.peer_address() << std::endl;
}