#include "memory.hpp"
#include <jce/load.hpp>
#include <jce/save.hpp>


void AlphaZero::ai::Memory::save(int version)
{
	this->save("test.memory");
}

void AlphaZero::ai::Memory::save(char filename[])
{
	std::ofstream out(filename, std::ios::binary);
	jce::save(out, this->memory);
}

void AlphaZero::ai::Memory::load(int version)
{
	this->load("test.memory");
}

void AlphaZero::ai::Memory::load(char filename[])
{
	std::ifstream in(filename, std::ios::binary);
	jce::load(in, this->memory);
}