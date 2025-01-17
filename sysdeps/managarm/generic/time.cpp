
#include <bits/ensure.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include <hel-syscalls.h>
#include <hel.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>

#include "posix.frigg_bragi.hpp"

struct TrackerPage {
	uint64_t seqlock;
	int32_t state;
	int32_t padding;
	int64_t refClock;
	int64_t baseRealtime;
};

extern thread_local TrackerPage *__mlibc_clk_tracker_page;

namespace mlibc {

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	// This implementation is inherently signal-safe.
	if (clock == CLOCK_MONOTONIC || clock == CLOCK_MONOTONIC_RAW
	    || clock == CLOCK_MONOTONIC_COARSE) {
		uint64_t tick;
		HEL_CHECK(helGetClock(&tick));
		*secs = tick / 1000000000;
		*nanos = tick % 1000000000;
	} else if (clock == CLOCK_REALTIME) {
		cacheFileTable();

		// Start the seqlock read.
		auto seqlock = __atomic_load_n(&__mlibc_clk_tracker_page->seqlock, __ATOMIC_ACQUIRE);
		__ensure(!(seqlock & 1));

		// Perform the actual loads.
		auto ref = __atomic_load_n(&__mlibc_clk_tracker_page->refClock, __ATOMIC_RELAXED);
		auto base = __atomic_load_n(&__mlibc_clk_tracker_page->baseRealtime, __ATOMIC_RELAXED);

		// Finish the seqlock read.
		__atomic_thread_fence(__ATOMIC_ACQUIRE);
		__ensure(__atomic_load_n(&__mlibc_clk_tracker_page->seqlock, __ATOMIC_RELAXED) == seqlock);

		// Calculate the current time.
		uint64_t tick;
		HEL_CHECK(helGetClock(&tick));
		__ensure(
		    tick >= (uint64_t)__mlibc_clk_tracker_page->refClock
		); // TODO: Respect the seqlock!
		tick -= ref;
		tick += base;
		*secs = tick / 1000000000;
		*nanos = tick % 1000000000;
	} else if (clock == CLOCK_PROCESS_CPUTIME_ID) {
		mlibc::infoLogger() << "\e[31mmlibc: clock_gettime does not support the CPU time clocks"
		                       "\e[39m"
		                    << frg::endlog;
		*secs = 0;
		*nanos = 0;
	} else if (clock == CLOCK_BOOTTIME) {
		uint64_t tick;
		HEL_CHECK(helGetClock(&tick));

		*secs = tick / 1000000000;
		*nanos = tick % 1000000000;
	} else {
		mlibc::panicLogger() << "mlibc: Unexpected clock " << clock << frg::endlog;
	}
	return 0;
}

int sys_clock_getres(int clock, time_t *secs, long *nanos) {
	(void)clock;
	(void)secs;
	(void)nanos;
	mlibc::infoLogger() << "mlibc: clock_getres is a stub" << frg::endlog;
	return 0;
}

int sys_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
	if (which != ITIMER_REAL) {
		mlibc::infoLogger() << "mlibc: setitimers other than ITIMER_REAL are unsupported"
		                    << frg::endlog;
		return EINVAL;
	}

	managarm::posix::SetIntervalTimerRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_which(which);
	req.set_value_sec(new_value->it_value.tv_sec);
	req.set_value_usec(new_value->it_value.tv_usec);
	req.set_interval_sec(new_value->it_interval.tv_sec);
	req.set_interval_usec(new_value->it_interval.tv_usec);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::SetIntervalTimerResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	__ensure(resp.error() == managarm::posix::Errors::SUCCESS);

	if (old_value) {
		old_value->it_value.tv_sec = resp.value_sec();
		old_value->it_value.tv_usec = resp.value_usec();
		old_value->it_interval.tv_sec = resp.interval_sec();
		old_value->it_interval.tv_usec = resp.interval_usec();
	}

	return 0;
}

} // namespace mlibc
