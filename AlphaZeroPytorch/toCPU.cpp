#include <ai/model.hpp>
#include <iostream>


int main()
{
	int version;
	std::cout << "type version ID: ";
	std::cin >> version;

	std::cout << "building model" << std::endl;
	AlphaZero::ai::Model model = AlphaZero::ai::Model();
	std::cout << "loading" << std::endl;
	model.load_version(version);
	std::cout << "transfering" << std::endl;
	model.toCPU();
	std::cout << "saving" << std::endl;
	model.save_version(version);
	std::cout << "done" << std::endl;
}