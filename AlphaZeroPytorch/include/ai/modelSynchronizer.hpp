#pragma once

#include <mutex>
#include <thread>
#include <thread>
#include <iostream>
#include <memory>
#include "model.hpp"

// mostly useless

namespace AlphaZero
{
	namespace ai
	{
		// class that allows model prediction to be heald untill a certain amount of MCTS threads requested and evaluation or skiped
		class ModelSynchronizer
		{
		private: std::vector<std::unique_ptr<Model>> models;
		private: unsigned short pos = 0;
		private: std::mutex modelGetMutex;

		public: ModelSynchronizer(std::vector<char*> devices);

			   // add Data vor evaluation 
		public: void addData(ModelData* data);
		private: Model* getModel();

			   
		public: void copyModel(ModelSynchronizer*);
		public: void fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop);

		public: void save_as_current();
		public: void save_version(unsigned int version);
		public: void save_to_file(char* filename);
		public: void jce_save_current(char* filename);
		public: void load_current();
		public: void load_version(unsigned int version);
		public: void load_from_file(char* filename);
		public: void jce_load_from_file(char* filename);
		public: void synchronizeModels();

		public: std::pair<float, torch::Tensor>predict(std::shared_ptr<Game::GameState> state, size_t idx=0);
		public: void predict(ModelData* data, size_t idx=0);
		public: void predict(std::list<ModelData*> data, size_t idx=0);
		};
	}
	namespace test
	{
		namespace ModelSynchronizer
		{
			std::thread addTestData(ai::ModelData* data, ai::ModelSynchronizer* sync);
			void _addTestData(ai::ModelData* data, ai::ModelSynchronizer* sync);
		}
	}
}

inline AlphaZero::ai::ModelSynchronizer::ModelSynchronizer(std::vector<char*> devices)
{
	for (auto const& device : devices)
	{
		this->models.push_back(std::make_unique<Model>(device));
	}
	this->synchronizeModels();
}
inline void AlphaZero::ai::ModelSynchronizer::addData(ModelData* _data)
{
	/*_data->value = 2;
	std::list<ModelData*>data_l;
	data_l.push_back(_data);*/
	this->getModel()->predict(_data);
}
inline AlphaZero::ai::Model* AlphaZero::ai::ModelSynchronizer::getModel()
{
	this->modelGetMutex.lock();
	auto outputModel = this->models[this->pos].get();
	this->pos++;
	if (this->pos >= this->models.size())
	{
		this->pos = 0;
	}
	this->modelGetMutex.unlock();
	return outputModel;
}
inline void AlphaZero::ai::ModelSynchronizer::copyModel(ModelSynchronizer* syncher)
{
	for (auto const& model : this->models)
	{
		model->copyModel(syncher->models[0].get());
	}
}
inline void AlphaZero::ai::ModelSynchronizer::fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop)
{
	this->models[0]->fit(batch, run, trainingLoop);
}
inline void AlphaZero::ai::ModelSynchronizer::save_as_current()
{
	this->models[0]->save_as_current();
}
inline void AlphaZero::ai::ModelSynchronizer::save_to_file(char* filename)
{
	this->models[0]->save_to_file(filename);
}
inline void AlphaZero::ai::ModelSynchronizer::jce_save_current(char* filename)
{
	this->models[0]->jce_save_current(filename);
}
inline void AlphaZero::ai::ModelSynchronizer::save_version(unsigned int version)
{
	this->models[0]->save_version(version);
}
inline void AlphaZero::ai::ModelSynchronizer::load_current()
{
	this->models[0]->load_current();
	this->synchronizeModels();
}
inline void AlphaZero::ai::ModelSynchronizer::load_from_file(char* filename)
{
	this->models[0]->load_from_file(filename);
	this->synchronizeModels();
}
inline void AlphaZero::ai::ModelSynchronizer::jce_load_from_file(char* filename)
{
	this->models[0]->jce_load_from_file(filename);
}
inline void AlphaZero::ai::ModelSynchronizer::load_version(unsigned int version)
{
	this->models[0]->load_version(version);
	this->synchronizeModels();
}
inline void AlphaZero::ai::ModelSynchronizer::synchronizeModels()
{
	auto copyFrom = this->models[0].get();
	for (auto const& model : this->models)
	{
		if (model.get() != copyFrom)
		{
			model->copyModel(copyFrom);
		}
	}
}
inline void AlphaZero::ai::ModelSynchronizer::predict(ModelData* data, size_t idx)
{
	this->models[idx]->predict(data);
}
inline void AlphaZero::ai::ModelSynchronizer::predict(std::list<ModelData*> data, size_t idx)
{
	this->models[idx]->predict(data);
}
inline std::pair<float, torch::Tensor> AlphaZero::ai::ModelSynchronizer::predict(std::shared_ptr<Game::GameState> state, size_t idx)
{
	return this->models[idx]->predict(state);
}


inline std::thread AlphaZero::test::ModelSynchronizer::addTestData(ai::ModelData* data, ai::ModelSynchronizer* sync)
{
	std::thread thread(_addTestData, data, sync);
	return thread;
}

inline void AlphaZero::test::ModelSynchronizer::_addTestData(ai::ModelData* data, ai::ModelSynchronizer* sync)
{
	sync->addData(data);
}