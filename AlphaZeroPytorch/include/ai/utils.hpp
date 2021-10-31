#pragma once
#include <vector>

namespace AlphaZero {
	namespace ai {
		// Softams function inp is an iteratable of numbers
		template<typename T>
		void softMaxNoDiv(T& inp);

		template<typename T>
		void linmax(T& inp);

		template<typename T>
		T getSumm(std::vector<T>& val);
	}
}

inline int ipow(int base, int exp)
{
	int result = 1;
	for (;;)
	{
		if (exp & 1)
			result *= base;
		exp >>= 1;
		if (!exp)
			break;
		base *= base;
	}

	return result;
}

template<typename T>
inline void AlphaZero::ai::softMaxNoDiv(T& inp)
{
	for (auto& val : inp)
	{
		if (val != 0)
		{
			val = ipow(2, val);
		}
	}
}

template<typename T>
void AlphaZero::ai::linmax(T& inp)
{
	float sum = 0;
	for (auto const& idx : inp)
	{
		sum = sum + idx;
	}
	for (auto& idx : inp)
	{
		idx = idx / sum;
	}
	return;
}

template<typename T>
T AlphaZero::ai::getSumm(std::vector<T>& val)
{
	T out = 0;
	for (const T& value : val)
	{
		out = out + value;
	}
	return out;
}