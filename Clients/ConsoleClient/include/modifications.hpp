#pragma once

namespace modifications
{
	inline void bottomLable()
	{
		std::cout << "0 1 2 3 4 5 6" << std::endl;
	}
	inline int allowedActionModification(int action)
	{
		return action % 7;
	}
}