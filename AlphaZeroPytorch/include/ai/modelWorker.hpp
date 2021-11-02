#pragma once

#include "model.hpp"
#include <game/game.hpp>
#include <list>
#include <mutex>
#include <thread>

namespace AlphaZero
{
	namespace ai
	{
		class Node;
		class Edge;
		class modelManager;

		class WorkerData
		{
		public: WorkerData(std::pair<Node*, std::list<Edge*>> edges);
		public: ~WorkerData();
		public: Node* node;
		public: std::vector<Edge*> edges;
		public: float value = 0.0f;
		public: std::vector<float> polys;
		};

		class modelWorker
		{
		private: bool done = false;
		private: bool keepRunning = true;
		public: bool currentlyRunning = false;
		public: std::thread thread;
		public: std::mutex runMutex;
		public: std::unique_ptr<Model> model;
		public: char* deviceName;

		public: modelManager* manager;
		public: modelWorker(char* deviceName, modelManager* manager);
		public: ~modelWorker();
		public: void mainLoop();
		public: void terminate();
		private: static void caler(modelWorker*);
		private: void spinLock();
		};
		class modelManager
		{
		public: std::list<WorkerData*> toRun;
		public: std::list<WorkerData*> hasRun;
		private: std::mutex runMutex, hasRunMutex, workerMutex;
		public: std::vector<modelWorker*> workers;
			   
		public: void unlockAll();
		public: modelManager();
		public: ~modelManager();
		public: void addNode(WorkerData* node);
		public: void addFinishedNode(WorkerData* data);
		public: std::list<WorkerData*> getNodes();
		public: std::list<WorkerData*> getFinishedData();
		public: void waitForData();

		public: bool isDone();

		public: void addWorker(modelWorker* worker);
		public: void removeWorker(modelWorker* worker);

		public: void fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop);
		public: void save_as_current();
		public: void load_current();
		public: void save_version(unsigned int version);
		public: void load_version(unsigned int version);
		public: void save_to_file(char*);
		public: void load_from_file(char*);
		public: void copyModel(modelManager* manager);
		};
	}
}

inline void AlphaZero::ai::modelManager::unlockAll()
{
	/*for (auto& val : this->workers)
	{
		val->runMutex.unlock();
	}*/
}

inline void AlphaZero::ai::modelWorker::caler(modelWorker* worker)
{
	worker->model = std::make_unique<Model>();
	worker->model->moveTo(c10::Device(worker->deviceName));
	worker->done = true;
	while (worker->keepRunning)
	{
		if (worker->manager->toRun.size())
		{
			worker->mainLoop();
		}
	}
}

inline bool AlphaZero::ai::modelManager::isDone()
{
	if (this->toRun.size() > 0)
	{
		return false;
	}
	if (this->hasRun.size() > 0)
	{
		return false;
	}
	for (auto worker : this->workers)
	{
		if (worker->currentlyRunning)
		{
			return false;
		}
	}
	return true;
}

inline AlphaZero::ai::modelManager::modelManager()
{
}

inline AlphaZero::ai::modelManager::~modelManager()
{
	for (auto& worker : this->workers)
	{
		delete worker;
	}
}

inline void AlphaZero::ai::modelManager::save_as_current()
{
	this->workers.front()->model->save_as_current();
}
inline void AlphaZero::ai::modelManager::load_current()
{
	for (auto val : this->workers)
		val->model->load_current();
}
inline void AlphaZero::ai::modelManager::save_version(unsigned int version)
{
	this->workers.front()->model->save_version(version);
}
inline void AlphaZero::ai::modelManager::load_version(unsigned int version)
{
	for (auto val : this->workers)
		val->model->load_version(version);
}
inline void AlphaZero::ai::modelManager::save_to_file(char* data)
{
	this->workers.front()->model->save_to_file(data);
}
inline void AlphaZero::ai::modelManager::load_from_file(char* data)
{
	for (auto val : this->workers)
		val->model->load_from_file(data);
}
inline void AlphaZero::ai::modelManager::copyModel(modelManager* manager)
{
	for (auto val : this->workers)
		val->model->copyModel(manager->workers.front()->model.get());
}