#pragma once
#include <iostream>

namespace jce
{
	namespace consoleUtils
	{
		void render_progress_bar(float progress, bool persistand=false);
	}
    void format(char out[], const char in[], int number);
}

inline void jce::consoleUtils::render_progress_bar(float progress, bool persistant)
{
#if true
    if (progress <= 1.0) {
        int barWidth = 70;

        std::cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        if (persistant) 
        {
            std::cout << "] " << int(progress * 100.0) << std::endl;
        }
        else
        {
            std::cout << "] " << int(progress * 100.0) << " %\r";
        }
        std::cout.flush();
    }
#else
    return;
#endif
}