
#include <bits/ensure.h>
#include <bits/errors.hpp>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#include <frg/allocation.hpp>
#include <hel-syscalls.h>
#include <hel.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-pipe.hpp>
#include <protocols/posix/supercalls.hpp>

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
	SignalGuard sguard;

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

namespace {

bool timerThreadInit = false;

struct PosixTimerContext {
	int setupSem = 0;
	int workerSem = 0;
	sigevent *sigev;
};

struct TimerHandle {
	uint64_t id;
	int notify_type;
	pthread_t thread = {};
};

void timer_handle(int, siginfo_t *, void *) {}

void *timer_setup(void *arg) {
	auto ctx = reinterpret_cast<PosixTimerContext *>(arg);

	sigset_t set = {};
	sigaddset(&set, SIGTIMER);

	// wait for parent setup to be complete
	while (__atomic_load_n(&ctx->setupSem, __ATOMIC_RELAXED) == 0)
		;
	pthread_testcancel();

	// copy out the function and argument, as the lifetime of the context ends with
	// incrementing workerSem
	auto notify = ctx->sigev->sigev_notify_function;
	union sigval val = ctx->sigev->sigev_value;

	// notify the parent that the context can be dropped
	__atomic_store_n(&ctx->workerSem, 1, __ATOMIC_RELEASE);

	siginfo_t si;
	int signo;

	while (true) {
		pthread_testcancel();
		while (sys_sigtimedwait(&set, &si, nullptr, &signo))
			;
		pthread_testcancel();
		if (si.si_code == SI_TIMER && signo == SIGTIMER)
			notify(val);
	}

	return nullptr;
}

} // namespace

int sys_timer_create(clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res) {
	SignalGuard sguard;

	if (!res)
		return EINVAL;

	managarm::posix::TimerCreateRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_clockid(clk);

	// TODO: pass sigev_value
	if (!evp) {
		req.set_sigev_signo(SIGALRM);
		req.set_sigev_tid(sys_gettid());
	} else if (evp->sigev_notify == SIGEV_NONE) {
		req.set_sigev_signo(0);
		req.set_sigev_tid(0);
	} else if (evp->sigev_notify == SIGEV_SIGNAL) {
		req.set_sigev_signo(evp->sigev_signo);
		req.set_sigev_tid(sys_gettid());
	} else if (evp->sigev_notify == SIGEV_THREAD_ID) {
		req.set_sigev_signo(evp->sigev_signo);
		req.set_sigev_tid(evp->sigev_notify_thread_id);
	} else if (evp->sigev_notify == SIGEV_THREAD) {
		if (!timerThreadInit) {
			struct sigaction sa{};
			sa.sa_flags = SA_SIGINFO | SA_RESTART;
			sa.sa_sigaction = timer_handle;
			sys_sigaction(SIGTIMER, &sa, 0);
			timerThreadInit = true;
		}

		pthread_attr_t attr;
		if (evp->sigev_notify_attributes)
			attr = *evp->sigev_notify_attributes;
		else
			pthread_attr_init(&attr);

		int ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if (ret)
			return ret;

		PosixTimerContext context{};
		context.sigev = evp;

		// mask for all signals except the libc-reserved RT signal range
		sigset_t mask = {
#if ULONG_MAX == 0xFFFF'FFFF
		    0x7FFF'FFFF, 0xFFFF'FFFC
#else
		    0xFFFF'FFFC'7FFF'FFFF
#endif
		};
		// but also mask SIGTIMER
		sigaddset(&mask, SIGTIMER);
		HelWord original_set;
		uint64_t unused;

		HEL_CHECK(helSyscall2_2(
		    kHelObserveSuperCall + posix::superSigMask,
		    SIG_BLOCK,
		    *reinterpret_cast<const HelWord *>(&mask),
		    &original_set,
		    &unused
		));

		pthread_t pthread;
		ret = pthread_create(&pthread, &attr, timer_setup, &context);

		// restore previous signal mask
		HEL_CHECK(helSyscall2_2(
		    kHelObserveSuperCall + posix::superSigMask, SIG_SETMASK, original_set, &unused, &unused
		));

		if (ret)
			return ret;

		req.set_sigev_signo(SIGTIMER);
		req.set_sigev_tid(reinterpret_cast<Tcb *>(pthread)->tid);
		infoLogger() << "mlibc: timer_create: created timer thread "
		             << reinterpret_cast<Tcb *>(pthread)->tid << frg::endlog;

		auto [offer, send_req, recv_resp] = exchangeMsgsSync(
		    getPosixLane(),
		    helix_ng::offer(
		        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
		    )
		);
		HEL_CHECK(offer.error());
		HEL_CHECK(send_req.error());
		HEL_CHECK(recv_resp.error());

		managarm::posix::TimerCreateResponse<MemoryAllocator> resp(getSysdepsAllocator());
		resp.ParseFromArray(recv_resp.data(), recv_resp.length());
		if (resp.error() != managarm::posix::Errors::SUCCESS) {
			pthread_cancel(pthread);
			__atomic_store_n(&context.setupSem, 1, __ATOMIC_RELEASE);
			return resp.error() | toErrno;
		}

		// notify worker that setup is complete
		__atomic_store_n(&context.setupSem, 1, __ATOMIC_RELEASE);
		// await worker setup to let the context go out of scope
		while (__atomic_load_n(&context.workerSem, __ATOMIC_RELAXED) == 0)
			;

		*res = frg::construct<TimerHandle>(
		    getSysdepsAllocator(), resp.timer_id(), evp->sigev_notify, pthread
		);
		return 0;
	} else {
		mlibc::infoLogger() << "mlibc: timer_create: unsupported sigevent type "
		                    << evp->sigev_notify << frg::endlog;
		return EINVAL;
	}

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::TimerCreateResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	*res = frg::construct<TimerHandle>(
	    getSysdepsAllocator(), resp.timer_id(), evp ? evp->sigev_notify : SIGEV_SIGNAL, pthread_t{}
	);
	return 0;
}

int sys_timer_settime(
    timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old
) {
	SignalGuard sguard;

	auto timerHandle = reinterpret_cast<TimerHandle *>(t);
	managarm::posix::TimerSetRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_timer(timerHandle->id);
	req.set_flags(flags);
	req.set_value_sec(val->it_value.tv_sec);
	req.set_value_nsec(val->it_value.tv_nsec);
	req.set_interval_sec(val->it_interval.tv_sec);
	req.set_interval_nsec(val->it_interval.tv_nsec);

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::TimerSetResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (old) {
		old->it_value.tv_sec = resp.value_sec();
		old->it_value.tv_nsec = resp.value_nsec();
		old->it_interval.tv_sec = resp.interval_sec();
		old->it_interval.tv_nsec = resp.interval_nsec();
	}

	return 0;
}

int sys_timer_gettime(timer_t t, struct itimerspec *val) {
	SignalGuard sguard;

	auto timerHandle = reinterpret_cast<TimerHandle *>(t);
	managarm::posix::TimerGetRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_timer((timerHandle->id));

	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::TimerGetResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	if (resp.error() != managarm::posix::Errors::SUCCESS)
		return resp.error() | toErrno;

	if (val) {
		val->it_value.tv_sec = resp.value_sec();
		val->it_value.tv_nsec = resp.value_nsec();
		val->it_interval.tv_sec = resp.interval_sec();
		val->it_interval.tv_nsec = resp.interval_nsec();
	}

	return 0;
}

int sys_timer_delete(timer_t t) {
	SignalGuard sguard;

	auto timerHandle = reinterpret_cast<TimerHandle *>(t);

	if (timerHandle->notify_type == SIGEV_THREAD) {
		pthread_cancel(timerHandle->thread);
		pthread_kill(timerHandle->thread, SIGTIMER);
	}

	managarm::posix::TimerDeleteRequest<MemoryAllocator> req(getSysdepsAllocator());
	req.set_timer(timerHandle->id);
	auto [offer, send_req, recv_resp] = exchangeMsgsSync(
	    getPosixLane(),
	    helix_ng::offer(
	        helix_ng::sendBragiHeadOnly(req, getSysdepsAllocator()), helix_ng::recvInline()
	    )
	);
	HEL_CHECK(offer.error());
	HEL_CHECK(send_req.error());
	HEL_CHECK(recv_resp.error());

	managarm::posix::TimerDeleteResponse<MemoryAllocator> resp(getSysdepsAllocator());
	resp.ParseFromArray(recv_resp.data(), recv_resp.length());
	return resp.error() | toErrno;
}

} // namespace mlibc
