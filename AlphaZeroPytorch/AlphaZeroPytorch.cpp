﻿// AlphaZeroPytorch.cpp : Defines the entry point for the application.
//

#include "AlphaZeroPytorch.h"
#include <ai/playGame.hpp>
#include <jce/string.hpp>
#include <io.hpp>
#include <chrono>
#include <thread>
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

void createFolder(char name[])
{
#ifndef UNIX
	std::filesystem::create_directories(name);
#else
	const char* foo = "mkdir -p";
	char* full_text;
	full_text = malloc(strlen(foo) + strlen(name) + 1);
	strcpy(full_text, foo);
	strcat(full_text, name);
	std::cout << full_text << std::endl;
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

	createFolder("logs/c++");
	createFolder("logs/games");

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
	agent->model->load_current();

	AlphaZero::Server::TCPServer server (agent);
	server.mainLoop();

#elif OPSMode == 2
	std::cout << "\33[1;31mUsing Test Server! \n\tset OPSMode to 1 for server if not testing\33[0m" << std::endl;
	AlphaZero::Server::TestServer server (PORT);
	server.mainLoop();
#endif
#if ProfileLogger
	debug::Profiler::profiler.log();
#endif 
	return 0;
}
