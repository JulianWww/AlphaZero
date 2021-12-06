#include <Server/eloClient.hpp>
#include <ai/playGame.hpp>

void evaluateAgent(int agent, int games, AlphaZero::elo::eloClient const& elo)
{
	std::vector<char*> devices = { DEVICES };
	std::shared_ptr<AlphaZero::ai::Agent> lastAgent = std::make_shared<AlphaZero::ai::Agent>(devices);
	std::shared_ptr<AlphaZero::ai::Agent> currentAgent = std::make_shared<AlphaZero::ai::Agent>(devices);

	if (agent -1 > 0)
	{
		lastAgent->model->load_version(agent - 1);
	}
	if (agent > 0)
	{
		lastAgent->model->load_version(agent);
	}
}

int main()
{
	AlphaZero::elo::eloClient elo;
	std::cout << elo.send(2, 2) << std::endl;
	return 1;
}