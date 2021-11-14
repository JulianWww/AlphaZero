// AlphaZeroPytorch.cpp : Defines the entry point for the application.
//

#include "AlphaZeroPytorch.h"
#include <ai/playGame.hpp>
#include <jce/string.hpp>
#include <io.hpp>
#include <chrono>
#include <thread>
#include <string>
#ifndef UNIX
#include <filesystem>
#endif
#if OPSMode == 1 || OPSMode == 2
#include <Server/server.hpp>
#endif


inline void wait(int time)
{
	std::chrono::seconds dura(time);
	std::this_thread::sleep_for(dura);
}

void createFolder(std::string str)
{
	createFolder(str.c_str());
}

void createFolder(char name[])
{
#ifndef UNIX
	std::filesystem::create_directories(name);
#else
	const char* foo = "mkdir -p ";
	char* full_text = new char[100];
	strcpy(full_text, foo);
	strcat(full_text, name);
	system(full_text);
#endif
}

void inline train(int arg)
{
	char folder[100];

	sprintf(folder, "models/run_%d", runVersion);
	createFolder(folder);

	sprintf(folder, "memory/run_%d", runVersion);
	createFolder(folder);

	sprintf(folder, "logs/c++");
	createFolder(folder);

	sprintf(folder, "logs/games");
	createFolder(folder);

	std::cout << "started training" << std::endl;

	AlphaZero::ai::train(arg);
}


int main(int argc, char ** argv)
{
	/*std::ofstream out("out.txt");
	std::streambuf* coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!*/

	if (torch::cuda::cudnn_is_available()) {
		std::cout << "\33[1;32mcuDNN is available\33[0m" << std::endl;
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