#include <iostream>
#include <intrin.h> // __rdtsc()
#include <limits>
#include "timer.hpp"

long long rdtsc() noexcept
{
	return __rdtsc();
}

using IntType = uint_fast32_t;

volatile IntType foo; // prevent dead code removal

template <typename T>
void bar(T lo, T hi) noexcept
{
	for (auto i = lo; i < hi; ++i) {
		foo = i;
	}
}

int main()
{
	constexpr auto MINVAL = std::numeric_limits<IntType>::min();
	constexpr auto MAXVAL = std::numeric_limits<IntType>::max();
	HRTimer timer;
	std::cout << "freq=" << timer.frequency() << '\n';

	for (int loop = 0; loop < 5; ++loop) {
		stopwatch(timer, bar<IntType>, MINVAL, MAXVAL);
		//cout << "rdtsc=" << rdtsc() << " ";
		std::cout << "ticks=" << timer.ticks() << " usecs=" << std::fixed << timer.useconds() << " msecs=" << timer.mseconds() << " secs=" << timer.seconds() << '\n';
	}
}
