#pragma once
#include <iostream>

namespace jce
{
    namespace consoleUtils
    {
        // render a progress bar to screen, copied from the JCE reposetory to avoid adding it as a library dependency
        // JCE: https://github.com/JulianWww/JCE
        void render_progress_bar(float progress, bool persistand = false);
    }
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
#endif
}