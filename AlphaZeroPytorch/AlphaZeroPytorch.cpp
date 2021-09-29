// AlphaZeroPytorch.cpp : Defines the entry point for the application.
//

#include "AlphaZeroPytorch.h"
#include <ai/playGame.hpp>

using namespace std;

int main()
{
	std::cout << "started training" << std::endl;
	if (torch::cuda::cudnn_is_available()) {
		std::cout << "\33[1;32mcudnn is available\33[0m" << std::endl;	
	}
	AlphaZero::ai::train(-1);
	return 0;
}
