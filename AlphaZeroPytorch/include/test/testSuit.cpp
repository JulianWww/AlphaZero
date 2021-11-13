#include "testSuit.hpp"
#include <stdio.h>
#include <ai/memory.hpp>
#include <ai/modelSynchronizer.hpp>


void AlphaZero::test::runTests()
{
	std::cout << "running Test" << std::endl;
	testModelData();
	testCoppying();
	testSave();
	testLossLog();
	//testTraining();
}


void AlphaZero::test::testCoppying()
{
	std::cout << "Testing Model coppying ...\t";

	auto modelA = std::make_shared<ai::Agent>();
	auto modelB = std::make_shared<ai::Agent>();

	modelB->model->copyModel(modelA->model);
	printSuccess(compareAgents(modelA, modelB));
}

void AlphaZero::test::testSave()
{
	std::cout << "Testing Model save ...\t\t";

	auto modelA = std::make_shared<ai::Agent>();
	auto modelB = std::make_shared<ai::Agent>();

	char folder[] = "temp.torch";
	modelA->model->save_to_file(folder);
	modelB->model->load_from_file(folder);

	remove("temp.torch");

	printSuccess(compareAgents(modelA, modelB));
}

void AlphaZero::test::testLossLog()
{
	std::cout << "Testing loss Logger ...\t\t";
#if LossLogger
	auto log1 = debug::log::lossLogger();
	log1.addValue(1.0f, 2.3f);
	log1.addValue(5.234f, 9834.2345789f);
	log1.newBatch();
	log1.addValue({ 44.634f, 234.4344f });

	char folder[] = "temp.log.bin";
	log1.save(folder);
	auto log2 = debug::log::lossLogger(folder);
	remove(folder);

	printSuccess(log2 == log1);
#else
	std::cout << "\33[1;33mDeactivated\33[0m" << std::endl;
#endif
}

void AlphaZero::test::testModelData()
{
	std::cout << "Testing model prediction ...\t";
	float epsilon = 0.000001f;
	auto model = std::make_shared<ai::Agent>();
	auto states = std::vector<std::shared_ptr<Game::GameState>>({ getRandomState(), getRandomState() });

	ai::ModelSynchronizer syncher(states.size(), model->model.get());

	auto nodes = std::vector<ai::Node*>();
	auto data = std::list<ai::ModelData*>();
	auto holders = std::vector<std::thread>();

	for (auto const& state : states)
	{
		auto node = new ai::Node(state);
		nodes.push_back(node);
		data.push_back(new ai::ModelData(node));
		holders.push_back(std::thread(ModelSynchronizer::_addTestData, data.back(), &syncher));
	}

	auto iter = data.begin();
	bool isValid = true;
	for (auto& holder : holders)
	{
		holder.join();
	}
	for (size_t idx = 0; idx < data.size(); idx++)
	{
		auto a = model->predict(states[idx]);
		auto error = torch::mse_loss(torch::from_blob(a.second.data(), a.second.size()), (*iter)->polys);
		//std::cout << error << std::endl;
		iter++;
	}

	printSuccess(isValid);
}

void AlphaZero::test::testTraining()
{
	auto model = std::make_shared<ai::Agent>();
	auto state = getRandomState();
	auto vec = jce::vector::gen(42, 0);
	vec[0] = 1;
	std::cout << model->model->predict(state) << std::endl;
	std::shared_ptr<ai::Memory> memory = std::make_shared<ai::Memory>();
	for (size_t loop = 0; loop < 10; loop++)
	{
		while (memory->tempMemory.size() < Training_batch * Training_loops)
		{
			//state = getRandomState();
			memory->commit(state, vec);
		}
		memory->updateMemory(0, 0);
		model->fit(memory, Training_loops);
	}
	std::cout << model->model->predict(state) << std::endl;
	return;
}