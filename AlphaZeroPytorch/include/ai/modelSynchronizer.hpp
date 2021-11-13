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
		private: unsigned short waitingThreads = 0;
		private: std::unique_ptr<std::thread> thread;
		private: std::list<ModelData*> data;
		private: std::mutex dataAddMutex, waitingThreadsMutex;

		private: std::condition_variable NNProcessingSynchronizer;

		public: ModelSynchronizer(ai::Model* model);
		public: ~ModelSynchronizer();

			   // add Data vor evaluation 
		public: void addData(ModelData* data);

			  // function used for synchronization 
		private: void waitForData(ModelData* _data);
		private: void predictData();
		public: static void mainloop(ModelSynchronizer* _this);

		private: void addWaitingThread();
		private: void removeWaitingThread();
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
	//this->waitForDataLocker.notify_all();
	this->thread->join();
}

inline void AlphaZero::ai::ModelSynchronizer::addData(ModelData* _data)
{
	_data->value = 2;
	//this->waitForData(_data);
	this->model->predict(std::list<ModelData*>{data});
}

inline void AlphaZero::ai::ModelSynchronizer::waitForData(ModelData* _data)
{
	this->addWaitingThread();
	std::mutex mutex;
	std::unique_lock<std::mutex>lock(mutex);

	while (_data->value == 2)
	{
		this->dataAddMutex.lock();
		this->data.push_back(_data);
		this->dataAddMutex.unlock();

		this->NNProcessingSynchronizer.wait(lock);
	}
	this->removeWaitingThread();
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
	this->dataAddMutex.unlock();

	this->model->predict(tmpData);
	this->data.clear();
	this->NNProcessingSynchronizer.notify_all();
}

inline void AlphaZero::ai::ModelSynchronizer::mainloop(ModelSynchronizer* _this)
{
	while (_this->isStillValid)
	{
		if (_this->data.size() == _this->waitingThreads && _this->data.size())
		{
			_this->predictData();
		}
	}
}

inline void AlphaZero::ai::ModelSynchronizer::addWaitingThread()
{
	this->waitingThreadsMutex.lock();
	this->waitingThreads++;
	this->waitingThreadsMutex.unlock();
}

inline void AlphaZero::ai::ModelSynchronizer::removeWaitingThread()
{
	this->waitingThreadsMutex.lock();
	this->waitingThreads--;
	this->waitingThreadsMutex.unlock();
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