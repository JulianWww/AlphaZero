﻿// AlphaZeroPytorch.cpp : Defines the entry point for the application.
//

#include "AlphaZeroPytorch.h"
#include <ai/playGame.hpp>
#include <jce/string.hpp>
#include <chrono>
#include <thread>


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
	torch::enableRecordFunction();
	torch::autograd::profiler::RecordProfile("log/c++/pytorchProfiler.trace");
	if (torch::cuda::cudnn_is_available()) {
		std::cout << "\33[1;32mcuDNN is available\33[0m" << std::endl;
		auto device = c10::Device("cuda:0");
		std::cout << "got device" << std::endl;
		std::cout << device << std::endl;
	}
	else {
		std::cout << "\33[1;31mWarning: cuDNN is unavailable, consider using a CUDA enabled GPU\33[0m" << std::endl;
	}
	
	train(-1);
#if ProfileLogger
	debug::Profiler::profiler.log();
#endif 
	return 0;
}
