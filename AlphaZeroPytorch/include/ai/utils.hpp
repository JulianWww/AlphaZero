#pragma once
#include <ostream>
#include <istream>
#include <vector>

namespace AlphaZero {
	namespace ai {
		// Softams function inp is an iteratable of numbers
		template<typename T>
		void softmax(T& inp);
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
	return;
}

/*

// This writes a vector of non-trivial data types.
template <class T>
inline void Write(std::ostream& s, const std::vector<T>& data)
{
	unsigned int len = data.size();
	s.write((char*)&len, sizeof(len));
	for (unsigned int i = 0; i < len; i++) {
		Write(s, data[i]);
	}
}

// This reads a vector of non-trivial data types.
template <class T>
inline void Read(std::istream& s, std::vector<T>& data)
{
	unsigned int len = 0;
	s.read((char*)&len, sizeof(len));
	data.resize(len);
	for (unsigned int i = 0; i < len; i++) {
		Read(s, data[i]);
	}
}
*/