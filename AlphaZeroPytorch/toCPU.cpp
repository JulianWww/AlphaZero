#include <ai/model.hpp>
#include <iostream>


int main()
{
	int version;
	std::cout << "type version ID: ";
	std::cin >> version;

	AlphaZero::ai::Model model = AlphaZero::ai::Model();
	model.load_version(version);
	model.toCPU();
	model.save_version(version);
}