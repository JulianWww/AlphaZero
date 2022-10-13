#include "testSuit.hpp"
#include <stdio.h>
#include <ai/memory.hpp>
#include <ai/playGame.hpp>
#include <timer.hpp>

void AlphaZero::test::runTests()
{
	std::cout << "running Test" << std::endl;
	testCoppying();
	testSave();
	testJCESave();
	testLossLog();
	if (torch::cuda::cudnn_is_available() || randomModel)
		testModelSpeed();
}


void AlphaZero::test::testCoppying()
{
	std::cout << "Testing Model coppying ...\t\t";

	auto modelA = std::make_shared<ai::Agent>(DEVICE);
	auto modelB = std::make_shared<ai::Agent>(DEVICE);

	modelA->model->save_as_current();

	modelB->model->copyModel(modelA->model.get());
	printSuccess(compareAgents(modelA, modelB));
}

void AlphaZero::test::testSave()
{
	std::cout << "Testing Model save ...\t\t\t";

	auto modelA = std::make_shared<ai::Agent>(DEVICE);
	auto modelB = std::make_shared<ai::Agent>(DEVICE);

	char folder[] = "temp.torch";
	modelA->model->save_to_file(folder);
	modelB->model->load_from_file(folder);

	remove("temp.torch");

	printSuccess(compareAgents(modelA, modelB));
}

void AlphaZero::test::testJCESave()
{
	std::cout << "Testing Model jce save ...\t\t";

	auto modelA = std::make_shared<ai::Agent>(DEVICE);
	auto modelB = std::make_shared<ai::Agent>(DEVICE);

	char folder[] = "temp.torch";
	modelA->model->jce_save_current(folder);
	modelB->model->jce_load_from_file(folder);

	remove("temp.torch");

	printSuccess(compareAgents(modelA, modelB));
}

void AlphaZero::test::testLossLog()
{
	std::cout << "Testing loss Logger ...\t\t\t";
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

void AlphaZero::test::testTraining()
{
	auto model = std::make_shared<ai::Agent>(DEVICE);
	auto state = getRandomState();
	auto vec = jce::vector::gen(42, 0);
	vec[0] = 1;
	std::cout << model->model->predict(state) << std::endl;
	std::shared_ptr<ai::Memory> memory = std::make_shared<ai::Memory>();
	for (size_t loop = 0; loop < 10; loop++)
	{
		ai::TemporaryMemory tmpMem(true);
		while (tmpMem.tempMemory.size() < Training_batch * Training_loops)
		{
			//state = getRandomState();
			tmpMem.commit(state, vec);
		}
		memory->updateMemory(0, 0, &tmpMem);
		model->fit(memory, Training_loops);
	}
	std::cout << model->model->predict(state) << std::endl;
	return;
}

void AlphaZero::test::testModelSpeed()
{
	std::cout << "testing Prediction speed ...\t\t";
	std::shared_ptr<ai::Memory> memory = std::make_shared<ai::Memory>();
	std::shared_ptr<ai::Agent> bestAgent = std::make_shared<ai::Agent>(DEVICE);
	std::shared_ptr<Game::Game> game = std::make_shared<Game::Game>();
	char nameBuff[100];
	utils::Timer timer;
	timer.reset();
	auto score = ai::playGames_inThreads(game.get(), bestAgent.get(), bestAgent.get(), memory.get(), Turnement_probabiliticMoves, TurneyEpochs, TurneyThreads, nameBuff, 0, true);
	std::cout << timer.elapsed() << std::endl;
}