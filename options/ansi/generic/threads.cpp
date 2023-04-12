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
