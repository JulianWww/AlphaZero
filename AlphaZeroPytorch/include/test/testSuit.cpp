#include "testSuit.hpp"
#include <stdio.h>


void AlphaZero::test::runTests()
{
	std::cout << "running Test" << std::endl;
	testModelData();
	testCoppying();
	testSave();
	testLossLog();
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
	auto states = std::vector<std::shared_ptr<Game::GameState>>({ getRandomState(), getRandomState(), getRandomState(), getRandomState(), getRandomState(), getRandomState() });

	auto nodes = std::vector<ai::Node*>();
	auto data = std::list<ai::ModelData*>();
	for (auto const& state : states)
	{
		auto node = new ai::Node(state);
		nodes.push_back(node);
		data.push_back(new ai::ModelData(node));
	}
	model->predict(data);

	auto iter = data.begin();
	bool isValid = true;
	for (size_t idx = 0; idx < data.size(); idx++)
	{
		auto a = model->predict(states[idx]);
		if (std::abs((*iter)->value - a.first) > epsilon)
		{
			std::cout << std::endl << (*iter)->value << std::endl << a.first << std::endl;
			isValid = false;
		}
//		(*iter)->print();

		for (size_t idx = 0; idx < action_count; idx++)
		{
			if (std::abs((*iter)->polys[idx].item<float>() - a.second[idx]) > epsilon)
			{
				std::cout << std::endl << (*iter)->polys[idx].item<float>() << std::endl << a.second[idx] << std::endl;
				isValid = false;
			}
		}
		iter++;
	}

	printSuccess(isValid);
}