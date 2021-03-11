#include "timer.hpp"
#ifdef HRTIMER_USE_QPC
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdlib> // exit()
#else
#include <chrono>
#endif

namespace hrtimer_detail
{
#ifdef HRTIMER_USE_QPC
	long long query_frequency() noexcept
	{
		LARGE_INTEGER frequency;
		if (!QueryPerformanceFrequency(&frequency)) {
			exit(static_cast<int>(GetLastError()));
		}
		return frequency.QuadPart;
	}
	long long query_counter() noexcept
	{
		LARGE_INTEGER counter;
		if (!QueryPerformanceCounter(&counter)) {
			exit(static_cast<int>(GetLastError()));
		}
		return counter.QuadPart;
	}
#else
	using namespace std::chrono;
	long long query_frequency() noexcept
	{
		return duration_cast<steady_clock::duration>(seconds(1)).count();
	}
	long long query_counter() noexcept
	{
		return steady_clock::now().time_since_epoch().count();
	}
#endif
}

HRTimer::value_type HRTimer::frequency_ = 0;

HRTimer::HRTimer() : cumulative_{ 0 }, time_{ 0 }, running_{ false }
{
	if (frequency_ == 0) {
		frequency_ = hrtimer_detail::query_frequency();
	}
}

void HRTimer::start() noexcept // start and reset
{
	cumulative_ = 0;
	running_ = true;
	time_ = hrtimer_detail::query_counter();
}

void HRTimer::restart() noexcept // start (no reset)
{
	if (!running_) {
		running_ = true;
		time_ = hrtimer_detail::query_counter();
	}
}

void HRTimer::stop() noexcept // stop and accumulate
{
	if (running_) {
		const auto now = hrtimer_detail::query_counter();
		cumulative_ += now - time_;
		running_ = false;
	}
}

void HRTimer::reset() noexcept // zero
{
	if (running_) {
		time_ = hrtimer_detail::query_counter();
	}
	cumulative_ = 0;
}

HRTimer::value_type HRTimer::ticks() noexcept // raw ticks
{
	if (running_) {
		const auto now = hrtimer_detail::query_counter();
		cumulative_ += now - time_;
		time_ = now;
	}
	return cumulative_;
}

double HRTimer::nseconds() noexcept // nanoseconds
{
	return (ticks() * 1E9) / frequency_;
}

double HRTimer::useconds() noexcept // microseconds
{
	return (ticks() * 1E6) / frequency_;
}

double HRTimer::mseconds() noexcept // milliseconds
{
	return (ticks() * 1E3) / frequency_;
}

double HRTimer::seconds() noexcept // seconds
{
	return static_cast<double>(ticks()) / frequency_;
}
