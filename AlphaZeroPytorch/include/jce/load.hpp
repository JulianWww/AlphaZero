#pragma once

#include <fstream>
#include <list>
#include <vector>
#include <bitset>
#include <string>
#include <ai/memory.hpp>
#include <torch/torch.h>
#include <game/game.hpp>

#define BasicLoad(in, data) (in.read((char*)&data, sizeof(data)))

namespace jce
{
	// custom part

	// load GameState from file
	void load(std::ifstream& in, std::shared_ptr<AlphaZero::Game::GameState>& state);

	// load Memory Element from file
	void load(std::ifstream& in, std::shared_ptr<AlphaZero::ai::MemoryElement>& element);

	template<typename key, typename T>
	void load(std::ifstream& in, torch::OrderedDict<key, T>& map);

	// load Tensor from file
	template<typename T=float>
	void load(std::ifstream& in, torch::Tensor& tensor);

	// end custom part

	template<typename key, typename T>
	void load(std::ifstream& in, std::unordered_map<key, T>& map);

	// load pair from file
	template<typename T, typename T2>
	void load(std::ifstream& in, std::pair<T, T2>& data);
	
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
	// load char from file
	void load(std::ifstream& in, char*& data);
	// load int64_t from file
	void load(std::ifstream& in, int64_t& data);
	// load string from file
	void load(std::ifstream& in, std::string& data);
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

template<typename key, typename T>
inline void jce::load(std::ifstream& in, torch::OrderedDict<key, T>& map)
{
	size_t size;
	jce::load(in, size);
	for (size_t idx = 0; idx < size; idx++)
	{
		key _key;
		T _item;
		jce::load(in, _key);
		jce::load(in, _item);
		map.insert(_key, _item);
	}
}

template<typename T>
inline void jce::load(std::ifstream& in, torch::Tensor& tensor)
{
	std::vector<int64_t> vec;
	int64_t fullSize = 1;
	jce::load(in, vec);
	for (size_t value : vec) { fullSize *= value; }
	tensor = torch::zeros({ fullSize });
	T val, last;
	for (size_t idx = 0; idx < fullSize; idx++)
	{
		jce::load(in, val);
		tensor[idx] = val;
		last = val;
	}
	tensor = torch::reshape(tensor, vec);
}

template<typename key, typename T>
inline void jce::load(std::ifstream& in, std::unordered_map<key, T>& map)
{
	size_t size;
	jce::load(in, size);
	for (size_t idx = 0; idx < size; idx++)
	{
		key _key;
		T _item;
		jce::load(in, _key);
		jce::load(in, _item);
		map.insert({ _key, _item });
	}
}

template<typename T, typename T2>
inline void jce::load(std::ifstream& in, std::pair<T, T2>& data)
{
	jce::load(in, data.first);
	jce::load(in, data.second);
}

template<typename T>
inline void jce::load(std::ifstream& in, std::list<T>& data)
{
	load_listVec(in, data);
}

template<typename T>
inline void jce::load(std::ifstream& in, std::vector<T>& data)
{
	load_listVec(in, data);
}

template<size_t size>
inline void jce::load(std::ifstream& in, std::bitset<size>& data)
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
inline void jce::load(std::ifstream& in, int64_t& data) { BasicLoad(in, data); }

inline void jce::load(std::ifstream& in, std::string& data)
{
	char* c_arr;
	jce::load(in, c_arr);
	data = std::string(c_arr);
}

inline void jce::load(std::ifstream& in, char*& data)
{
	std::vector<char> data_vec;
	while (true)
	{
		char next;
		in.read(&next, 1);
		data_vec.push_back(next);
		if (next == NULL)
		{
			break;
		}
	}
	data = new char[data_vec.size()];
	auto pos = data;
	for (auto const& value : data_vec)
	{
		(*pos) = value;
		pos++;
	}
}
