#pragma once

#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <bitset>
#include <ai/memory.hpp>

#define BasicSave(data, out) (out.write((char*)&data, sizeof(data)))

namespace jce
{
	// custom part
	
	// save GameState to file
	void save(std::ofstream& out, std::shared_ptr<AlphaZero::Game::GameState> const& state);

	// save memory element to file
	void save(std::ofstream& out, std::shared_ptr<AlphaZero::ai::MemoryElement> const& element);

	// end custom part

	template<typename T>
	//save list to ofstream
	void save(std::ofstream& out, std::list<T> const& data);

	template<typename T>
	//save vector to file
	void save(std::ofstream& out, std::vector<T> const& data);

	//save bitset to file
	template <size_t T>
	void save(std::ofstream& out, std::bitset<T> const& data);

	template<typename T>
	//the actuall saving function for vectors and lists
	void save_listVec(std::ofstream& out, T const& data);

	// save int to file
	void save(std::ofstream& out, int const& data);
	// save unsigned int to file
	void save(std::ofstream& out, unsigned int const& data);
	// save size_t to to file
	void save(std::ofstream& out, size_t const& data);
	// save float to file
	void save(std::ofstream& out, float const& data);
	// save double to file
	void save(std::ofstream& out, double const& data);
}

inline void jce::save(std::ofstream& out, std::shared_ptr<AlphaZero::ai::MemoryElement> const& element)
{
	jce::save(out, element->value);
	jce::save(out, element->state);
	jce::save(out, element->av);
}

inline void jce::save(std::ofstream& out, std::shared_ptr<AlphaZero::Game::GameState> const& state)
{
	jce::save(out, state->gameBoard);
	jce::save(out, state->player);
}

template<typename T>
inline void jce::save(std::ofstream& out, std::list<T> const& data)
{
	save_listVec(out, data);
}

template <typename T>
inline void jce::save(std::ofstream& out, std::vector<T> const& data)
{
	save_listVec(out, data);
}

template<size_t T>
void jce::save(std::ofstream& out, std::bitset<T> const& data)
{
	std::bitset<8> temp;
	size_t tempVal;
	for (size_t idx = 0; idx < T; idx = idx + 8)
	{
		for (size_t pos = 0; pos < 8 && pos + idx < T; pos++) {
			temp.set(pos, data[pos + idx]);
		}
		tempVal = temp.to_ullong();
		out.write((char*)&tempVal, 1);
	}
}

template <typename T>
inline void jce::save_listVec(std::ofstream& out, T const & data)
{
	jce::save(out, data.size());
	for (auto const& data : data)
	{
		jce::save(out, data);
	}
}

inline void jce::save(std::ofstream& out, int const& data) { BasicSave(data, out); }
inline void jce::save(std::ofstream& out, unsigned int const& data) { BasicSave(data, out); }
inline void jce::save(std::ofstream& out, size_t const& data) { BasicSave(data, out); }
inline void jce::save(std::ofstream& out, float const& data) { BasicSave(data, out); }
inline void jce::save(std::ofstream& out, double const& data) { BasicSave(data, out); }


