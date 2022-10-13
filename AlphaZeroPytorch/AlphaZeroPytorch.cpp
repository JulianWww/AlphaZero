// AlphaZeroPytorch.cpp : Defines the entry point for the application.
//

#include "AlphaZeroPytorch.h"
#include <ai/playGame.hpp>
#include <io.hpp>
#include <chrono>
#include <thread>
#include "makeFiles.hpp"

// load everything and start training
int main(int argc, char ** argv)
{
	/*std::ofstream out("out.txt");
	std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!*/

	if (torch::cuda::cudnn_is_available()) 
	{
		std::cout << "\33[1;32mcuDNN is available\33[0m" << std::endl;
	}
	else if (torch::hasXLA())
	{
		std::cout << "\33[1;32mXLa is available\33[0m" << std::endl;
	}
	else 
	{
		std::cout << "\33[1;31mWarning: cuDNN is unavailable, consider using a CUDA enabled GPU\33[0m" << std::endl;
	}
  std::cout << "running on" << DEVICE << std::endl;

	std::cout << std::endl << "started training" << std::endl;
	createFolders();
	AlphaZero::ai::train(-1);
#if ProfileLogger
	debug::Profiler::profiler.log();
#endif 
	return 0;
}