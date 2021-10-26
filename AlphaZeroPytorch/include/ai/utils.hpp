#pragma once
#include <ostream>
#include <istream>
#include <vector>

namespace AlphaZero {
	namespace ai {
		// Softams function inp is an iteratable of numbers
		template<typename T>
		void softmax(T& inp);

		template<typename T>
		void linmax(T& inp);

		template<typename T>
		T getSumm(std::vector<T>& val);
	}
}

template<typename T>
inline void AlphaZero::ai::softmax(T& inp){
	typedef float number;

	number m = -10e100;
	for (number const& z : inp){
		if (m < z) {
			m = z;
		}
	}

	number sum = 0.0;
	for (number const& z : inp) {
		sum += exp(z - m);
	}

	number constant = m + log(sum);
	for (number& z : inp) {
		z = exp(z - constant);
	}
	throw "Depricated function";
	return;
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