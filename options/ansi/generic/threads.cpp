#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <errno.h>
#include <mlibc-config.h>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/thread.hpp>
#include <mlibc/threads.hpp>
#include <threads.h>
#include <time.h>

void call_once(once_flag *flag, void (*func)(void)) {
	mlibc::thread_once(flag, func);
}

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
	int res = mlibc::thread_create(thr, nullptr, reinterpret_cast<void *>(func), arg, true);

	if(!res) {
		return thrd_success;
	}

	return (res == ENOMEM) ? thrd_nomem : thrd_error;
}

int thrd_equal(thrd_t t1, thrd_t t2) {
	if(t1 == t2) {
		return 1;
	}
	return 0;
}

thrd_t thrd_current(void) {
	return reinterpret_cast<thrd_t>(mlibc::get_current_tcb());
}

int thrd_sleep(const struct timespec *duration, struct timespec *remaining) {
	if(!mlibc::sys_sleep) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_sleep()");
	}

	if (duration->tv_nsec < 0 || duration->tv_nsec >= 1'000'000'000) {
#if __MLIBC_POSIX_OPTION
		errno = EINVAL;
#endif
		return -2;
	}

	struct timespec tmp = *duration;
	int e = mlibc::sys_sleep(&tmp.tv_sec, &tmp.tv_nsec);

	switch (e) {
		case 0:
			return 0;
		case EINTR:
			if (remaining)
				*remaining = tmp;
#if __MLIBC_POSIX_OPTION
			errno = EINTR;
#endif
			return -1;
		default:
			return -2;
	}
}

void thrd_yield(void) {
	if(mlibc::sys_yield) {
		mlibc::sys_yield();
	}else{
		// Missing sched_yield() is not an error.
		MLIBC_MISSING_SYSDEP();
	}
}

int thrd_detach(thrd_t thr) {
	return mlibc::thread_detach(thr) == 0 ? thrd_success : thrd_error;
}

int thrd_join(thrd_t thr, int *res) {
	if(mlibc::thread_join(thr, res) != 0) {
		return thrd_error;
	}

	return thrd_success;
}

__attribute__((__noreturn__)) void thrd_exit(int ret_val) {
	mlibc::thread_exit({.integer = ret_val});
}

int mtx_init(mtx_t *mtx, int type) {
	struct __mlibc_mutexattr attr;
	mlibc::thread_mutexattr_init(&attr);

	if(type & mtx_recursive) {
		mlibc::thread_mutexattr_settype(&attr, __MLIBC_THREAD_MUTEX_RECURSIVE);
	}

	int res = mlibc::thread_mutex_init(mtx, &attr) == 0 ? thrd_success : thrd_error;
	mlibc::thread_mutexattr_destroy(&attr);

	return res;
}

void mtx_destroy(mtx_t *mtx) {
	mlibc::thread_mutex_destroy(mtx);
}

int mtx_lock(mtx_t *mtx) {
	return mlibc::thread_mutex_lock(mtx) == 0 ? thrd_success : thrd_error;
}

int mtx_timedlock(mtx_t *__restrict mtx, const struct timespec *__restrict abstime) {
	auto ret = mlibc::thread_mutex_timedlock(mtx, abstime, CLOCK_REALTIME);

	switch (ret) {
		case 0:
			return thrd_success;
		case ETIMEDOUT:
			return thrd_timedout;
		default:
			return thrd_error;
	}
}

int mtx_trylock(mtx_t *mtx) {
	auto ret = mlibc::thread_mutex_trylock(mtx);
	switch (ret) {
		case 0:
			return thrd_success;
		case EBUSY:
			return thrd_busy;
		default:
			return thrd_error;
	}
}

int mtx_unlock(mtx_t *mtx) {
	return mlibc::thread_mutex_unlock(mtx) == 0 ? thrd_success : thrd_error;
}

int cnd_init(cnd_t *cond) {
	return mlibc::thread_cond_init(cond, nullptr) == 0 ? thrd_success : thrd_error;
}

void cnd_destroy(cnd_t *cond) {
	mlibc::thread_cond_destroy(cond);
}

int cnd_broadcast(cnd_t *cond) {
	return mlibc::thread_cond_broadcast(cond) == 0 ? thrd_success : thrd_error;
}

int cnd_wait(cnd_t *cond, mtx_t *mtx) {
	return mlibc::thread_cond_timedwait(cond, mtx, nullptr, 0) == 0 ? thrd_success : thrd_error;
}

int cnd_timedwait(cnd_t *__restrict cond, mtx_t *__restrict mutex, const struct timespec *__restrict abstime) {
	auto ret = mlibc::thread_cond_timedwait(cond, mutex, abstime, CLOCK_REALTIME);
	switch (ret) {
		case 0:
			return thrd_success;
		case ETIMEDOUT:
			return thrd_timedout;
		default:
			return thrd_error;
	}
}

int tss_create(tss_t *key, tss_dtor_t dtor) {
	return mlibc::thread_key_create(key, dtor) == 0 ? thrd_success : thrd_error;
}

void tss_delete(tss_t key) {
	mlibc::thread_key_delete(key);
}

void *tss_get(tss_t key) {
	return mlibc::thread_key_get(key);
}

int tss_set(tss_t key, void *val) {
	return mlibc::thread_key_set(key, val) == 0 ? thrd_success : thrd_error;
}
