#pragma once
#include <iostream>

namespace AlphaZero
{
	namespace test
	{
      // print collored success messages
		void printSuccess(bool val);
	}
}

inline void AlphaZero::test::printSuccess(bool val)
{
	if (val)
	{
		std::cout << "\33[32;1mSuccess\33[0m" << std::endl;
	}
	else
	{
		std::cout << "\33[31;1mFailed\33[0m" << std::endl;
	}
}