// AlphaZeroPytorch.cpp : Defines the entry point for the application.
//

#include "AlphaZeroPytorch.h"
#include <ai/playGame.hpp>


using namespace std;

int main()
{
	if (torch::cuda::cudnn_is_available()) {
		std::cout << "\33[1;32mcudnn is available\33[0m" << std::endl;	
	}
	else {
		std::cout << "\33[1;31mWarning: cuDNN is unavailable, consider using a CUDA enabled GPU\33[0m" << std::endl;
	}
	std::cout << "started training" << std::endl;
	AlphaZero::ai::train(-1);
	return 0;
}
