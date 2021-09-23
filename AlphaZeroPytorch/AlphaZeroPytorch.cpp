// AlphaZeroPytorch.cpp : Defines the entry point for the application.
//

#include "AlphaZeroPytorch.h"
#include <ai/playGame.hpp>

using namespace std;

int main()
{
	std::cout << "started training" << std::endl;
	AlphaZero::ai::train(-1);
	return 0;
}
