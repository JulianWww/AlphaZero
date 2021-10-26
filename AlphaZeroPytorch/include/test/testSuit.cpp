#include "testSuit.hpp"
#include <stdio.h>


void AlphaZero::test::runTests()
{
	std::cout << "running Test" << std::endl;
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