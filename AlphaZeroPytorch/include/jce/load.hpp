#pragma once

#include <fstream>
#include <list>
#include <vector>
#include <bitset>
#include <ai/memory.hpp>

#define BasicLoad(in, out) (in.read((char*)&data, sizeof(data)))

namespace jce
{
	// custom part

	// load GameState from file
	void load(std::ifstream& in, std::shared_ptr<AlphaZero::Game::GameState>& state);

	// load Memory Element from file
	void load(std::ifstream& in, std::shared_ptr<AlphaZero::ai::MemoryElement>& element);

	// end custom part
	
	// load list from file
	template<typename T>
	void load(std::ifstream& in, std::list<T>& data);

	// load vector from file
	template<typename T>
	void load(std::ifstream& in, std::vector<T>& data);

	//load bitset from file
	template<size_t size>
	void load(std::ifstream& in, std::bitset<size>& data);

	// the actuall loading of ints and vectors called by the load function
	template <typename T>
	void load_listVec(std::ifstream& in, T& data);

	// load int from file
	void load(std::ifstream& in, int& data);
	// load unsinged int from file
	void load(std::ifstream& in, unsigned int& data);
	// load size_t from file
	void load(std::ifstream& in, size_t& data);
	// load float from file
	void load(std::ifstream& in, float& data);
	// load double from file
	void load(std::ifstream& in, double& data);
}

inline void jce::load(std::ifstream& in, std::shared_ptr<AlphaZero::Game::GameState>& state)
{
	IDType board;
	int player;
	jce::load(in, board);
	jce::load(in, player);
	state.reset(new AlphaZero::Game::GameState(board, player));
}

inline void jce::load(std::ifstream& in, std::shared_ptr<AlphaZero::ai::MemoryElement>& element)
{
	element.reset(new AlphaZero::ai::MemoryElement());
	jce::load(in, element->value);
	jce::load(in, element->state);
	jce::load(in, element->av);
}

template<typename T>
void jce::load(std::ifstream& in, std::list<T>& data)
{
	load_listVec(in, data);
}

template<typename T>
void jce::load(std::ifstream& in, std::vector<T>& data)
{
	load_listVec(in, data);
}

template<size_t size>
void jce::load(std::ifstream& in, std::bitset<size>& data)
{
	char byte;
	for (size_t idx = 0; idx < size; idx = idx + 8)
	{
		in.read(&byte, 1);
		std::bitset<8> tempSet(byte);
		for (size_t pos = 0; pos < 8 && pos + idx < size; pos++)
		{
			data.set(pos + idx, tempSet[pos]);
		}
	}
}

template<typename T>
inline void jce::load_listVec(std::ifstream& in, T& data)
{
	size_t size;
	jce::load(in, size);
	data.resize(size);
	for (auto& val : data)
	{
		jce::load(in, val);
	}
}

inline void jce::load(std::ifstream& in, int& data){ BasicLoad(in, data); }
inline void jce::load(std::ifstream& in, unsigned int& data) { BasicLoad(in, data); }
inline void jce::load(std::ifstream& in, size_t& data) { BasicLoad(in, data); }
inline void jce::load(std::ifstream& in, float& data) { BasicLoad(in, data); }
inline void jce::load(std::ifstream& in, double& data) { BasicLoad(in, data); }