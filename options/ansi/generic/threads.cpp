#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/thread.hpp>
#include <mlibc/threads.hpp>
#include <threads.h>

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
	int res = mlibc::thread_create(thr, 0, reinterpret_cast<void *>(func), arg, true);

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

int thrd_sleep(const struct timespec *, struct timespec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void thrd_yield(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int thrd_detach(thrd_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int thrd_join(thrd_t thr, int *res) {
	if(mlibc::thread_join(thr, res) != 0) {
		return thrd_error;
	}

	return thrd_success;
}

__attribute__((__noreturn__)) void thrd_exit(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

int mtx_unlock(mtx_t *mtx) {
	return mlibc::thread_mutex_unlock(mtx) == 0 ? thrd_success : thrd_error;
}

int cnd_init(cnd_t *cond) {
	return mlibc::thread_cond_init(cond, 0) == 0 ? thrd_success : thrd_error;
}

void cnd_destroy(cnd_t *cond) {
	mlibc::thread_cond_destroy(cond);
}

int cnd_broadcast(cnd_t *cond) {
	return mlibc::thread_cond_broadcast(cond) == 0 ? thrd_success : thrd_error;
}

int cnd_wait(cnd_t *cond, mtx_t *mtx) {
	return mlibc::thread_cond_timedwait(cond, mtx, nullptr) == 0 ? thrd_success : thrd_error;
}
