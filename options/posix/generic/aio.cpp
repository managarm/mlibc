#include <aio.h>
#include <bits/ensure.h>
#include <errno.h>
#include <limits.h>

#include <mlibc/aio.hpp>
#include <mlibc/all-sysdeps.hpp>

int aio_read(struct aiocb *cb) {
	if (cb->aio_nbytes > SSIZE_MAX)
		return EINVAL;

	if (int e = mlibc::sysdep_or_enosys<AioRead>(cb); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int aio_write(struct aiocb *cb) {
	if (cb->aio_nbytes > SSIZE_MAX)
		return EINVAL;

	if (int e = mlibc::sysdep_or_enosys<AioWrite>(cb); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int aio_fsync(int op, struct aiocb *cb) {
	if (int e = mlibc::sysdep_or_enosys<AioFsync>(op, cb); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int lio_listio(
    int mode, struct aiocb *__restrict const list[], int n, struct sigevent *__restrict sevp
) {
	if (int e = mlibc::sysdep_or_enosys<AioListio>(mode, list, n, sevp); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int aio_suspend(const struct aiocb *const list[], int n, const struct timespec *ts) {
	if (int e = mlibc::sysdep_or_enosys<AioSuspend>(list, n, ts); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t aio_return(struct aiocb *cb) {
	mlibc::AioCbView cbv{cb};

	auto state = cbv.get_state();
	if (state != mlibc::AioOpState::success && state != mlibc::AioOpState::cancelled) {
		errno = EINVAL;
		return -1;
	}

	// invalidate the state so that aio_return can only be called once
	cbv.set_state(mlibc::AioOpState::invalid);
	ssize_t res = cb->__res;
	cbv.reset_operation_result();
	return res;
}

int aio_error(const struct aiocb *cb) {
	mlibc::ConstAioCbView cbv{cb};
	auto state = cbv.get_state();

	switch (state) {
		case mlibc::AioOpState::success:
			return cb->__err;
		case mlibc::AioOpState::in_progress:
			return EINPROGRESS;
		case mlibc::AioOpState::cancelled:
			return ECANCELED;
		case mlibc::AioOpState::invalid:
			errno = EINVAL;
			return -1;
	}
	__builtin_unreachable();
}

int aio_cancel(int fildes, struct aiocb *cb) {
	if (cb && fildes != cb->aio_fildes)
		return EINVAL;

	int result = 0;
	if (int e = mlibc::sysdep_or_enosys<AioCancel>(fildes, cb, &result); e) {
		errno = e;
		return -1;
	}
	return result;
}
