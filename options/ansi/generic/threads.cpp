#include <abi-bits/errno.h>
#include <mlibc/threads.hpp>
#include <threads.h>

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
	int res = mlibc::thread_create(thr, 0, reinterpret_cast<void *>(func), arg, true);

	if(!res) {
		return thrd_success;
	}

	return (res == ENOMEM) ? thrd_nomem : thrd_error;
}

int thrd_join(thrd_t thr, int *res) {
	if(mlibc::thread_join(thr, res) != 0) {
		return thrd_error;
	}

	return thrd_success;
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
