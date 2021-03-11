#ifndef HRTIMER_H
#define HRTIMER_H
#include <functional>
//#define HRTIMER_USE_QPC

class HRTimer {
public:
	using value_type = long long;

	HRTimer();

	void start() noexcept; // start and reset
	void restart() noexcept; // start (no reset)
	void stop() noexcept; // stop and accumulate
	void reset() noexcept; // zero

	value_type ticks() noexcept; // raw ticks
	double nseconds() noexcept; // nanoseconds
	double useconds() noexcept; // microseconds
	double mseconds() noexcept; // milliseconds
	double seconds() noexcept; // seconds

	static value_type frequency() noexcept { return frequency_; }

private:
	value_type cumulative_; // total time
	value_type time_; // split time
	bool running_; // current state

	static value_type frequency_; // ticks per second
};

template <typename Fn, typename... Args>
void stopwatch(HRTimer &timer, Fn &&func, Args &&...args)
{
	timer.start();
#if __cplusplus < 201703L
	std::forward<Fn>(func)(std::forward<Args>(args)...);
#else
	std::invoke(func, args...); // C++17
#endif
	timer.stop();
}
#endif // HRTIMER_H
