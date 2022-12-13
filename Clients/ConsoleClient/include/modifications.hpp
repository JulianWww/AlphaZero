#pragma once

namespace modifications
{
    // cout the actions below the game state
	inline void bottomLable()
	{
		std::cout << "0 1 2 3 4 5 6" << std::endl;
	}

    // convert Ai action to human action
	inline int allowedActionModification(int action)
	{
		return action % 7;
	}
}