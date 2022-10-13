#pragma once
#include <vector>

namespace jce {
	namespace vector {
    // generate a vector of ceratin size filled with a certain value
		template <typename T> 
		std::vector<T> gen(size_t size, T val);
	}
}

template<typename T>
inline std::vector<T> jce::vector::gen(size_t size, T val)
{
	std::vector<T> out(size);
	for (auto& item : out) {
		item = val;
	}
	return out;
}