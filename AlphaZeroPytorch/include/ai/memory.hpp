#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

#include <queue>
#include <game/game.hpp>
#include "utils.hpp"
#include <jce/vector.hpp>


namespace AlphaZero {
	namespace ai {
		class MemoryElement {
		public: int value;
		public: std::shared_ptr<Game::GameState>state;
		public: std::vector<float> av;
		public: MemoryElement(std::shared_ptr<Game::GameState>, std::vector<int>);
		public: MemoryElement();
		};
		class TemporaryMemory
		{
		public: bool active;
		public: TemporaryMemory(bool);
		public: std::vector<std::shared_ptr<MemoryElement>> tempMemory;
		public: void commit(std::shared_ptr<Game::GameState>, std::vector<int>&);
		};
		class Memory {
			//keep memory from doing annything
		private: std::mutex mu;
		public: bool active = true;
		public: TemporaryMemory getTempMemory();
		public: std::vector<std::shared_ptr<MemoryElement>> memory;
		public: Memory();
		public: void updateMemory(int player, int value, TemporaryMemory* memory);
		public: std::shared_ptr<MemoryElement> getState();
		public: void save(int version);
		public: void save();
		public: void save(char filename[]);
		public: void load(int version);
		public: void load();
		public: void load(char filename[]);
		public: void render();
		private: void updateMemory(int val, TemporaryMemory* memory);
		};
	}
}

inline AlphaZero::ai::TemporaryMemory::TemporaryMemory(bool val)
{
	this->active = val;
}

inline AlphaZero::ai::TemporaryMemory AlphaZero::ai::Memory::getTempMemory()
{
	AlphaZero::ai::TemporaryMemory memory(this->active);
	return memory;
}

inline AlphaZero::ai::MemoryElement::MemoryElement(std::shared_ptr<Game::GameState> _state, std::vector<int> _av)
{
	this->state = _state;
	float sum = (float)getSumm(_av);
	this->av = jce::vector::gen(_av.size(), 0.0f);
	for (size_t idx = 0; idx < this->av.size(); idx++)
	{
		float tmp = ((float)_av[idx]);
		this->av[idx] = tmp / sum;
	}
}
inline AlphaZero::ai::MemoryElement::MemoryElement()
{
}
inline AlphaZero::ai::Memory::Memory()
{
}
inline void AlphaZero::ai::TemporaryMemory::commit(std::shared_ptr<Game::GameState> state, std::vector<int>& av)
{
	if (this->active) {
		std::vector<std::pair<std::shared_ptr<Game::GameState>, std::vector<int>>> idents = Game::identities(state, av);
		for (auto const& data : idents) {
			tempMemory.push_back(std::make_shared<MemoryElement>(data.first, data.second));
		}
	}
}

inline void AlphaZero::ai::Memory::updateMemory(int player, int value, TemporaryMemory* memory)
{
	this->updateMemory(player * value, memory);
}

inline std::shared_ptr<AlphaZero::ai::MemoryElement> AlphaZero::ai::Memory::getState()
{
	unsigned long long idx = rand() % this->memory.size();
	std::shared_ptr<MemoryElement> element = this->memory[idx];
	this->memory.erase(this->memory.begin() + idx);
	return element;
}

inline void AlphaZero::ai::Memory::render()
{
	for (auto const& element : this->memory)
	{
		element->state->render();
	}
}

inline void AlphaZero::ai::Memory::updateMemory(int val, TemporaryMemory* memory)
{
	while (memory->tempMemory.size() > 0) {
		std::shared_ptr<MemoryElement>& element = memory->tempMemory.back();
		element->value = element->state->player * val;
		this->mu.lock();
		this->memory.push_back(element);
		this->mu.unlock();
		memory->tempMemory.pop_back();
	}
}
