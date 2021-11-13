#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include <list>
#include <thread>
#include <iostream>
#include "model.hpp"


namespace AlphaZero
{
	namespace ai
	{
		// class that allows model prediction to be heald untill a certain amount of MCTS threads requested and evaluation or skiped
		class ModelSynchronizer
		{
			// the total amount of courrent threads waiting on prediction
		public: size_t ThreadCounter = 0;
			  // the total amount of threads neaded befor prediction commences.
		public: size_t TotalRequiredThreads = 0;
		public: Model* model;
		private: std::list<ModelData*> data;
		private: std::mutex dataAddMutex, threadFinischMutex;
		private: std::condition_variable NNProcessingSynchronizer;

		public: ModelSynchronizer(size_t totalThreads, ai::Model* model);

			   // add Data vor evaluation 
		public: void addData(ModelData* data);
			  // makes TotalRequiredThreads one smaller
		public: void finischThread();

			  // function used for synchronization 
		private: void waitForData();
		private: void predictData();
		private: void DataCaller(bool hold=true);
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

inline AlphaZero::ai::ModelSynchronizer::ModelSynchronizer(size_t totalThreads, ai::Model* model)
{
	this->TotalRequiredThreads = totalThreads;
	this->model = model;
}

inline void AlphaZero::ai::ModelSynchronizer::addData(ModelData* _data)
{
	this->dataAddMutex.lock();

	this->data.push_back(_data);
	this->ThreadCounter++;
	this->DataCaller(true);
}

inline void AlphaZero::ai::ModelSynchronizer::waitForData()
{
	std::mutex mutex;
	std::unique_lock<std::mutex>lock(mutex);
	this->NNProcessingSynchronizer.wait(lock);
	return;
}

inline void AlphaZero::ai::ModelSynchronizer::predictData()
{
	assert(this->data.size() == this->TotalRequiredThreads);
	this->model->predict(this->data);
	this->data.clear();
	this->ThreadCounter = 0;
	this->NNProcessingSynchronizer.notify_all();
}

inline void AlphaZero::ai::ModelSynchronizer::finischThread()
{
	threadFinischMutex.lock();
	assert(this->TotalRequiredThreads > 0);
	this->TotalRequiredThreads--;

	this->dataAddMutex.lock();
	this->DataCaller(false);

	threadFinischMutex.unlock();
}

inline void AlphaZero::ai::ModelSynchronizer::DataCaller(bool hold)
{
	if (this->ThreadCounter >= this->TotalRequiredThreads)
	{
		this->predictData();
		this->dataAddMutex.unlock();
	}
	else
	{
		this->dataAddMutex.unlock();
		if (hold)
		{
			this->waitForData();
		}
	}
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