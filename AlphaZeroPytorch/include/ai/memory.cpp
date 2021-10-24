#include "memory.hpp"
#include "config.hpp"
#include <jce/load.hpp>
#include <jce/save.hpp>

inline void getName(char out[], int version, int run)
{
	sprintf(out, "memory/run_%d/V_%d.memory", run, version);
}

void AlphaZero::ai::Memory::save(int version)
{
	char nameBuff[100];
	getName(nameBuff, version, runVersion);
	this->save(nameBuff);
}

void AlphaZero::ai::Memory::save()
{
	this->save(-1);
}

void AlphaZero::ai::Memory::save(char filename[])
{
	std::ofstream out(filename, std::ios::binary);
	if (out.is_open())
	{
		jce::save(out, this->memory);
		out.close();
	}
	else
	{
		throw "Game saver file not opend.";
	}
}

void AlphaZero::ai::Memory::load(int version)
{
	char nameBuff[100];
	getName(nameBuff, version, runVersion);
	this->load(nameBuff);
}

void AlphaZero::ai::Memory::load()
{
	char nameBuff[100];
	getName(nameBuff, -1, runVersion);
	try{
		std::cout << "Loading memory from file ... ";
		this->load(nameBuff);
		std::cout << "\33[1;32mSuccess\33[0m" << std::endl;
	}
	catch (...)
	{
		std::cout << "\33[1;31mFailed!\33[0m" << std::endl;
	}
}

void AlphaZero::ai::Memory::load(char filename[])
{
	std::ifstream in(filename, std::ios::binary);
	if (in.is_open())
	{
		jce::load(in, this->memory);
		in.close();
	}
	else
	{
		throw "Game saver file not opend.";
	}
}