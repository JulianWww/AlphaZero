#include "modelWorker.hpp"
#include "MCTS.hpp"

AlphaZero::ai::WorkerData::~WorkerData()
{
	return;
}

AlphaZero::ai::modelWorker::modelWorker(char* _deviceName, modelManager* _manager)
{
	this->deviceName = _deviceName;
	this->manager = _manager;
	this->model = std::make_unique<Model>();
	this->model->to(c10::Device(_deviceName));
	this->thread = std::thread(caler, this);

	_manager->addWorker(this);
	this->spinLock();
}

AlphaZero::ai::modelWorker::~modelWorker()
{
	if (this->keepRunning)
	{
		this->terminate();
	}
	//delete this->deviceName;
}

void AlphaZero::ai::modelWorker::mainLoop()
{
	this->currentlyRunning = true;
	auto states = this->manager->getNodes();
	auto states_begin = states.begin();
	std::vector<Game::GameState*> data(states.size());
	for (size_t idx = 0; idx < states.size(); idx++)
	{
		data[idx] = (*states_begin)->node->state;
		states_begin++;
	}
	auto NNout = this->model->predict(data, (c10::Device(this->deviceName)));

	states_begin = states.begin();
	for (int idx = 0; idx < data.size(); idx++)
	{
		(*states_begin)->value = (NNout.first[idx][0]).item<float>();
		(*states_begin)->polys = std::vector<float>(action_count);
		for (size_t polyIdx = 0; polyIdx < action_count; polyIdx++)
		{
			(*states_begin)->polys[polyIdx] = NNout.second[idx][polyIdx].item<float>();
		}
		this->manager->addFinishedNode(*states_begin);
		states_begin++;
	}
	this->currentlyRunning = false;
}

void AlphaZero::ai::modelWorker::terminate()
{
	this->keepRunning = false;
	this->thread.join();
	this->manager->removeWorker(this);
}

AlphaZero::ai::WorkerData::WorkerData(std::pair<Node*, std::list<Edge*>> data)
{
	this->node = data.first;
	for (auto const& edge : data.second)
		this->edges.push_back(edge);

	this->value = 1.0f;
}

void AlphaZero::ai::modelWorker::spinLock()
{
	while (!this->done)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void AlphaZero::ai::modelManager::addNode(WorkerData* node)
{
	if (node->node->state->done)
	{
		node->value = std::get<0>(node->node->state->val);
		this->addFinishedNode(node);
	}
	else
	{
		this->runMutex.lock();
		this->toRun.push_back(node);
		this->unlockAll();
		this->runMutex.unlock();
	}
}

void AlphaZero::ai::modelManager::addFinishedNode(WorkerData* data)
{
	this->hasRunMutex.lock();
	this->hasRun.push_back(data);
	this->hasRunMutex.unlock();
}

std::list<AlphaZero::ai::WorkerData*> AlphaZero::ai::modelManager::getNodes()
{
	auto pnodes = std::list<WorkerData*>(this->toRun.begin(), this->toRun.end());
	this->runMutex.lock();
	this->toRun.clear();
	this->runMutex.unlock();
	return pnodes;
}

std::list<AlphaZero::ai::WorkerData*> AlphaZero::ai::modelManager::getFinishedData()
{
	this->hasRunMutex.lock();
	auto pdata = std::list<WorkerData*>(this->hasRun.begin(), this->hasRun.end());
	this->hasRun.clear();
	this->hasRunMutex.unlock();
	return pdata;
}

void AlphaZero::ai::modelManager::waitForData()
{
	while (this->hasRun.size() == 0)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}

void AlphaZero::ai::modelManager::addWorker(modelWorker* worker)
{
	this->workerMutex.lock();
	this->workers.push_back(worker);
	this->workerMutex.unlock();
}

void AlphaZero::ai::modelManager::removeWorker(modelWorker* worker)
{
	this->workerMutex.lock();
	this->workers.erase(std::find(this->workers.begin(), this->workers.end(), worker));
	this->workerMutex.unlock();
}

void  AlphaZero::ai::modelManager::fit(const std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>& batch, const unsigned short& run, const unsigned short& trainingLoop)
{
	this->workers.front()->model->fit(batch, run, trainingLoop);
	this->copyModel(this);
}