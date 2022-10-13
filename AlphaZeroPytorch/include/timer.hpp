#pragma once
#include <iostream>
#include <chrono>

namespace utils {
      // suxtom timer to mesure the time betwean two events
    class Timer
    {
    public:
        Timer();
        void reset();
        double elapsed() const;

    private:
        typedef std::chrono::high_resolution_clock clock_;
        typedef std::chrono::duration<double, std::ratio<1> > second_;
        std::chrono::time_point<clock_> beg_;
    };
};


inline utils::Timer::Timer()
{
    this->beg_ = clock_::now();
}

inline void utils::Timer::reset()
{
    this->beg_ = clock_::now();
}

inline double utils::Timer::elapsed() const
{
        return std::chrono::duration_cast<second_>(clock_::now() - beg_).count();
}
