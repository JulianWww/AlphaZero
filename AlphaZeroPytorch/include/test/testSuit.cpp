#include "testSuit.hpp"
#include <stdio.h>


void AlphaZero::test::runTests()
{
	std::cout << "running Test" << std::endl;
	testCoppying();
	testSave();
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

	const char folder[] = "temp.torch";
	modelA->model->save_to_file(folder);
	modelB->model->load_from_file(folder);

	remove("temp.torch");

	printSuccess(compareAgents(modelA, modelB));
}