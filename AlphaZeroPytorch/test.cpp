#include <test/testSuit.hpp>
#include <jce/save.hpp>
#include <jce/load.hpp>
#include "makeFiles.hpp"


int main(int argc, char** argv)
{
	createFolders();
	AlphaZero::test::runTests();
	return 1;
}