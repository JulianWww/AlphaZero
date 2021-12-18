#include <test/testSuit.hpp>
#include <jce/save.hpp>
#include <jce/load.hpp>
#include "makeFiles.hpp"
#include <vector>


int main(int argc, char** argv)
{
	//createFolders();
	//AlphaZero::test::runTests();

	std::vector<int> count;
	std::ifstream in("models/run_1/iterationCounter.jce");
	jce::load(in, count);
	in.close();
	for (auto const& val : count)
	{
		std::cout << val << ", ";
	}
	return 1;
}