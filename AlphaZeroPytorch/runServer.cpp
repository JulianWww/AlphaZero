#include <config.hpp>
#include <Server/server.hpp>
#include <game/game.hpp>



int main()
{
	std::vector<char*> devices = { DEVICES };
#if OPSMode == 1
	std::shared_ptr<AlphaZero::ai::Agent> agent = std::make_shared<AlphaZero::ai::Agent>(devices);
	agent->model->load_current();

	AlphaZero::Server::TCPServer server(agent);
	server.mainLoop();

#elif OPSMode == 2
	std::cout << "\33[1;31mUsing Test Server! \n\tset OPSMode to 1 for server if not testing\33[0m" << std::endl;
	AlphaZero::Server::TestServer server(PORT);
	server.mainLoop();
#endif
}