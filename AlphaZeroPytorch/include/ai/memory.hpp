#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

#include <queue>
#include <game/game.hpp>
#include "utils.hpp"


namespace AlphaZero {
	namespace ai {
		class MemoryElement {
		public: int value;
		public: std::shared_ptr<Game::GameState>state;
		public: std::vector<float> av;
		public: MemoryElement(std::shared_ptr<Game::GameState>, std::vector<float>);
		public: MemoryElement();
		};
		class Memory {
			//keep memory from doing annything
		public: bool active = true;
		public: std::vector<std::shared_ptr<MemoryElement>> tempMemory;
		public: std::vector<std::shared_ptr<MemoryElement>> memory;
		public: Memory();
		public: void commit(std::shared_ptr<Game::GameState>, std::vector<float>&);
		public: void updateMemory(int player, int value);
		public: std::shared_ptr<MemoryElement> getState();
		public: void save(int version);
		public: void save(char filename[]);
		public: void load(int version);
		public: void load(char filename[]);
		private: void updateMemory(int val);
		};
	}
}
inline AlphaZero::ai::MemoryElement::MemoryElement(std::shared_ptr<Game::GameState> _state, std::vector<float> _av)
{
	this->state = _state;
	this->av = _av;
}
inline AlphaZero::ai::MemoryElement::MemoryElement()
{
}
inline AlphaZero::ai::Memory::Memory()
{
}
inline void AlphaZero::ai::Memory::commit(std::shared_ptr<Game::GameState> state, std::vector<float>& av)
{
	if (active) {
		auto idents = Game::identities(state, av);
		for (auto const& data : idents) {
			tempMemory.push_back(std::make_shared<MemoryElement>(data.first, data.second));
		}
	}
}

inline void AlphaZero::ai::Memory::updateMemory(int player, int value)
{
	this->updateMemory(player * value);
}

inline std::shared_ptr<AlphaZero::ai::MemoryElement> AlphaZero::ai::Memory::getState()
{
	unsigned long long idx = rand() % this->memory.size();
	std::shared_ptr<MemoryElement> element = this->memory[idx];
	this->memory.erase(this->memory.begin() + idx);
	return element;
}

inline void AlphaZero::ai::Memory::updateMemory(int val)
{
	while (this->tempMemory.size() > 0) {
		std::shared_ptr<MemoryElement>& element = this->tempMemory.back();
		element->value = element->state->player * val;
		this->memory.push_back(element); // insert new element at random postion after modification ??
		this->tempMemory.pop_back();
	}
}
