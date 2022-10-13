#include <config.hpp>
#include <Server/server.hpp>
#include <game/game.hpp>


// start and run the Test Or AI server
int main()
{
#if OPSMode == 1
	AlphaZero::Server::TCPServer server;
	server.mainLoop();

#elif OPSMode == 2
	std::cout << "\33[1;31mUsing Test Server! \n\tset OPSMode to 1 for server if not testing\33[0m" << std::endl;
	AlphaZero::Server::TestServer server(PORT);
	server.mainLoop();
#endif
}