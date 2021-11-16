#pragma once

#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <bitset>
#include <ai/memory.hpp>
#include <torch/torch.h>

#define BasicSave(data, out) (out.write((char*)&data, sizeof(data)))

namespace jce
{
	// custom part
	
	// save GameState to file
	void save(std::ofstream& out, std::shared_ptr<AlphaZero::Game::GameState> const& state);

	// save memory element to file
	void save(std::ofstream& out, std::shared_ptr<AlphaZero::ai::MemoryElement> const& element);

	// save Tensor to file
	template<typename T=float>
	void save(std::ofstream& out, torch::Tensor const& tensor);

	template<typename key, typename T>
	void save(std::ofstream& out, torch::OrderedDict<key, T> const& map);

	// end custom part

	template<typename T, typename key>
	void save(std::ofstream& out, std::unordered_map<T, key>);

	template<typename T, typename T2>
	// save pair to file
	void save(std::ofstream& out, std::pair<T, T2>const& data);

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
	// save char to file
	void save(std::ofstream& out, const char* arr);
	// save int64_t to file
	void save(std::ofstream& out, const int64_t& data);
	// save string to file
	void save(std::ofstream& out, const std::string& data);
}

inline void jce::save(std::ofstream& out, std::shared_ptr<AlphaZero::ai::MemoryElement> const& element)
{
	jce::save(out, element->value);
	jce::save(out, element->state);
	jce::save(out, element->av);
}

template<typename T>
inline void jce::save(std::ofstream& out, torch::Tensor const& tensor)
{
	jce::save(out, tensor.sizes().vec());
	auto flatTensor = torch::flatten(tensor);
	for (size_t idx = 0; idx < flatTensor.size(0); idx++)
	{
		T val = flatTensor[idx].item<T>();
		jce::save(out, val);
	}
}

template<typename key, typename T>
inline void jce::save(std::ofstream& out, torch::OrderedDict<key, T> const& map)
{
	jce::save(out, map.size());
	for (auto& val : map)
	{
		jce::save(out, val.key());
		jce::save(out, val.value());
	}
}

inline void jce::save(std::ofstream& out, std::shared_ptr<AlphaZero::Game::GameState> const& state)
{
	jce::save(out, state->gameBoard);
	jce::save(out, state->player);
}

template<typename T, typename key>
inline void jce::save(std::ofstream& out, std::unordered_map<T, key> map)
{
	jce::save(out, map.size());
	for (auto const& val : map)
	{
		jce::save(out, val.first);
		jce::save(out, val.second);
	}
}

template<typename T, typename T2>
inline void jce::save(std::ofstream& out, std::pair<T, T2> const& data)
{
	jce::save(out, data.first);
	jce::save(out, data.second);
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
inline void jce::save(std::ofstream& out, std::bitset<T> const& data)
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
inline void jce::save(std::ofstream& out, const int64_t& data) { BasicSave(data, out); }

inline void jce::save(std::ofstream& out, const std::string& data)
{
	jce::save(out, data.c_str());
}

inline void jce::save(std::ofstream& out, const char* arr)
{
	size_t size = 1;
	auto iterator = arr;
	while (*iterator != NULL)
	{
		size++;
		iterator++;
	}
	out.write(arr, size);
}


