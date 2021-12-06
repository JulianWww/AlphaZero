#include <Server/eloClient.hpp>

int main()
{
	AlphaZero::elo::eloClient elo;
	std::cout << elo.send(1, 2) << std::endl;
	return 1;
}