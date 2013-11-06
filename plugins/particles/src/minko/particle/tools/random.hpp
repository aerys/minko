#pragma once

#include <random>
#include <chrono>

inline
float
rand01()
{
	static std::default_random_engine				generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_real_distribution<float>	distribution (0.0, 1.0);

	return distribution(generator);
}
