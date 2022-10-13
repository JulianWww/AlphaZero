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
      // One Element of memory
		class MemoryElement {

      // MCTS calculated reward r
		public: int value;

      // the state the element is affiliated with
		public: std::shared_ptr<Game::GameState>state;

      // the action values or policy computed by the MCTS using N
		public: std::vector<float> av;
		public: MemoryElement(std::shared_ptr<Game::GameState>, std::vector<int>);
		public: MemoryElement();
		};

    // Memory from a single game simulation waiting to be incorporated into the main memory buffer
		class TemporaryMemory
		{
      
      // weather or not to ignore all calls
		public: bool active;
		public: TemporaryMemory(bool);

      // list of memory elements consisting of everything traversed during one game simulation
		public: std::vector<std::shared_ptr<MemoryElement>> tempMemory;
		public: void commit(std::shared_ptr<Game::GameState>, std::vector<int>&);
		};

     // Global state memory used to pull states for training
		class Memory {
			//mutex used to ensure the mulitrheaded simulations integrate memory elements without causing memory corruption
		private: std::mutex mu;

      // weather or not to ignore all calls to the Momory
		public: bool active = true;

      // get TemporaryMemory with same active value
		public: TemporaryMemory getTempMemory();

      // list of all game states in the Memory
		public: std::vector<std::shared_ptr<MemoryElement>> memory;
		public: Memory();

      // update temporary memory with rewards and incorporated it into the main memory buffer
		public: void updateMemory(int player, int value, TemporaryMemory* memory);

      // pop a random state from the memory buffer
		public: std::shared_ptr<MemoryElement> getState();

      // save memory by version key
		public: void save(int version);
      // save the newest version
		public: void save();
      // save to specific file
		public: void save(char filename[]);
      // load memory by version key
		public: void load(int version);
      // load the newest version
		public: void load();
      // load from specific file
		public: void load(char filename[]);

      // prints out the entire memory buffer (not a lot of funn);
		public: void render();

      // subfunction used by update Memory
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
