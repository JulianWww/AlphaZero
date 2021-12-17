#include <Server/eloClient.hpp>
#include <ai/playGame.hpp>
#include <math.h> 

void evaluateAgent(int agent, int games, AlphaZero::elo::eloClient const& elo)
{
	std::cout << "evaluating elo for: " << agent << std::endl;
	std::vector<char*> devices = {DEVICES};
	std::shared_ptr<AlphaZero::ai::Agent> lastAgent = std::make_shared<AlphaZero::ai::Agent>(devices);
	std::shared_ptr<AlphaZero::ai::Agent> currentAgent = std::make_shared<AlphaZero::ai::Agent>(devices);
	AlphaZero::Game::Game* game = new AlphaZero::Game::Game();
	AlphaZero::ai::Memory* memory = new AlphaZero::ai::Memory();

	if (agent -1 > 0)
	{
		lastAgent->model->load_version(agent - 1);
	}
	if (agent > 0)
	{
		currentAgent->model->load_version(agent);
	}
	auto data = AlphaZero::ai::playGames_inThreads(game, lastAgent.get(), currentAgent.get(), memory, 2, 1, games, "eloEvaluation");

	int wins = data[currentAgent.get()];
	int losses = data[lastAgent.get()];
	int ties = games - wins - losses;

	int othersElo = elo.getElo(agent - 1);

	float score = ((float)wins + 0.5f * (float)ties)/((float)games);
	float Relo = (float)othersElo - log10((1 - score) / score) * 400;
	elo.setElo(agent, (int)Relo);

	std::cout << wins << " wins, " << ties << " ties and " << losses << " losses" << std::endl;
	std::cout << "win Ratio is : " << score << std::endl;
	std::cout << "new rating is: " << Relo << std::endl << std::endl;
	
	delete game;
	delete memory;
}

int main()
{
	AlphaZero::elo::eloClient elo;
	std::cout << elo.setElo(0, 100) << std::endl;

	int agent = 1;
	while (true)
	{
		evaluateAgent(agent, 40, elo);
		agent++;
	}
	return 1;
}