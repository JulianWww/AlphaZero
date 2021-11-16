#include <iostream>
#include <ai/model.hpp>

#define FILENAME "model.jce.bin"

int main(int argc, char** argv)
{
	int version=-2;
	std::cout << "What version do you want to convert -1 for current, -2 for inverse conversion: ";
	std::cin >> version;


	AlphaZero::ai::Model model("cpu");

	if (version >= 0)
	{
		model.load_version(version);
		model.jce_save_current(FILENAME);
	}
	else if (version == -1)
	{
		model.load_current();
		model.jce_save_current(FILENAME);
	}
	else if (version = -2)
	{
		model.jce_load_from_file(FILENAME);
		model.save_as_current();
	}
	else
	{
		return -1;
	}

	return 1;
}