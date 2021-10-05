// AlphaZeroPytorch.cpp : Defines the entry point for the application.
//

#include "AlphaZeroPytorch.h"
#include <ai/playGame.hpp>
#include <jce/string.hpp>
#include <chrono>
#include <thread>
#if OPSMode == 1
#include <Server/server.hpp>
#endif


inline void wait(int time)
{
	std::chrono::seconds dura(time);
	std::this_thread::sleep_for(dura);
}
void inline train(int arg)
{
	std::cout << "started training" << std::endl;
	AlphaZero::ai::train(arg);
}


int main()
{
	/*std::ofstream out("out.txt");
	std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!*/

	if (torch::cuda::cudnn_is_available()) {
		std::cout << "\33[1;32mcuDNN is available\33[0m" << std::endl;
		auto device = c10::Device("cuda:0");
		std::cout << "got device" << std::endl;
		std::cout << device << std::endl;
	}
	else {
		std::cout << "\33[1;31mWarning: cuDNN is unavailable, consider using a CUDA enabled GPU\33[0m" << std::endl;
	}
#if OPSMode == 0
	train(-1);
#elif OPSMode == 1
	//TODO remove game from initialization || its not technicly neded
	std::shared_ptr<AlphaZero::Game::Game> game = std::make_shared<AlphaZero::Game::Game>();

	std::shared_ptr<AlphaZero::ai::Agent> agent = std::make_shared<AlphaZero::ai::Agent>(game);
	agent->model->load_version(60);

	AlphaZero::Server::TCPServer server (agent);
	server.mainLoop();


#endif
#if ProfileLogger
	debug::Profiler::profiler.log();
#endif 
	return 0;
}
