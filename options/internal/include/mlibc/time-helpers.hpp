#pragma once

#include <abi-bits/clockid_t.h>
#include <bits/ansi/timespec.h>
#include <bits/ensure.h>
#include <frg/safe_int.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <time.h>

namespace mlibc {

// Converts the absolute time `abstime` to a relative time `reltime`.
// Returns false if the conversion failed (e.g. due to over-/underflow).
// If the absolute time has already passed, `reltime` is set to 0.
inline bool time_absolute_to_relative(const clockid_t clock, const struct timespec *__restrict abstime, struct timespec *reltime) {
	constexpr long nanos_per_second = 1'000'000'000;

	if (clock != CLOCK_REALTIME && clock != CLOCK_MONOTONIC) {
		mlibc::infoLogger() << "mlibc: time_relative_to_absolute() only supports CLOCK_REALTIME and CLOCK_MONOTONIC"
			<< frg::endlog;
		return false;
	}

	struct timespec now;
	if (mlibc::sysdep<ClockGet>(clock, &now.tv_sec, &now.tv_nsec))
		__ensure(!"sys_clock_get() failed");

	if (!frg::checked_sub(abstime->tv_sec, now.tv_sec, reltime->tv_sec))
		return false;
	if (!frg::checked_sub(abstime->tv_nsec, now.tv_nsec, reltime->tv_nsec))
		return false;

	// Check if abstime has already passed.
	if (reltime->tv_sec < 0 || (reltime->tv_sec == 0 && reltime->tv_nsec < 0)) {
		reltime->tv_sec = 0;
		reltime->tv_nsec = 0;
		return true;
	} else if (reltime->tv_nsec >= nanos_per_second) {
		reltime->tv_nsec -= nanos_per_second;
		if (!frg::checked_add(reltime->tv_sec, time_t{1}, reltime->tv_sec))
			return false;
		if (reltime->tv_nsec >= nanos_per_second)
			return false;
	} else if (reltime->tv_nsec < 0) {
		reltime->tv_nsec += nanos_per_second;
		if (!frg::checked_sub(reltime->tv_sec, time_t{1}, reltime->tv_sec))
			return false;
		if (reltime->tv_nsec < 0)
			return false;
	}

	return true;
}

} // namespace mlibc
