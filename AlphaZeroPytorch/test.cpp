#include <test/testSuit.hpp>
#include "makeFiles.hpp"


int main(int argc, char** argv)
{
	createFolders();
	AlphaZero::test::runTests();
	return 1;
}