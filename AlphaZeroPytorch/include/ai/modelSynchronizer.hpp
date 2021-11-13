#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>
#include <list>
#include <thread>
#include <iostream>
#include <memory>
#include "model.hpp"


namespace AlphaZero
{
	namespace ai
	{
		// class that allows model prediction to be heald untill a certain amount of MCTS threads requested and evaluation or skiped
		class ModelSynchronizer
		{
		public: Model* model;
		public: bool isStillValid = true;
		private: std::unique_ptr<std::thread> thread;
		private: std::list<ModelData*> data;
		private: std::mutex dataAddMutex, threadFinischMutex;

		private: std::condition_variable NNProcessingSynchronizer;
		public: std::condition_variable waitForDataLocker;

		public: ModelSynchronizer(ai::Model* model);
		public: ~ModelSynchronizer();

			   // add Data vor evaluation 
		public: void addData(ModelData* data);
			  // makes TotalRequiredThreads one smaller
		public: void finischThread();

			  // function used for synchronization 
		private: void waitForData(ModelData* _data);
		private: void predictData();
		public: static void mainloop(ModelSynchronizer* _this);
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

inline AlphaZero::ai::ModelSynchronizer::ModelSynchronizer(ai::Model* model)
{
	this->model = model;
	this->thread = std::make_unique<std::thread>(ModelSynchronizer::mainloop, this);
}

inline AlphaZero::ai::ModelSynchronizer::~ModelSynchronizer()
{
	this->isStillValid = false;
	waitForDataLocker.notify_all();
	this->thread->join();
}

inline void AlphaZero::ai::ModelSynchronizer::addData(ModelData* _data)
{
	_data->value = 2;
	this->waitForData(_data);
}

inline void AlphaZero::ai::ModelSynchronizer::waitForData(ModelData* _data)
{
	std::mutex mutex;
	std::unique_lock<std::mutex>lock(mutex);

	while (_data->value == 2)
	{
		this->waitForDataLocker.wait(lock);

		this->dataAddMutex.lock();
		this->data.push_back(_data);
		this->dataAddMutex.unlock();

		this->NNProcessingSynchronizer.wait(lock);
	}
	return;
}

inline void AlphaZero::ai::ModelSynchronizer::predictData()
{
	this->dataAddMutex.lock();
#if ModelLogger
	debug::log::modelLogger->info("model Predicted with {} states", this->data.size());
#endif
	std::list<ModelData*> tmpData(this->data.begin(), this->data.end());
	this->data.clear();
	this->waitForDataLocker.notify_all();
	this->dataAddMutex.unlock();

	this->model->predict(tmpData);
	this->data.clear();
	this->NNProcessingSynchronizer.notify_all();
}

inline void AlphaZero::ai::ModelSynchronizer::mainloop(ModelSynchronizer* _this)
{
	std::mutex mu;
	std::unique_lock<std::mutex> lock(mu);
	while (_this->isStillValid)
	{
		if(_this->data.size())
			_this->predictData();
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
}

inline void AlphaZero::ai::ModelSynchronizer::finischThread()
{
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