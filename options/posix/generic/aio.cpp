#include <aio.h>
#include <bits/ensure.h>
#include <errno.h>
#include <limits.h>

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
	ssize_t result = 0;
	if (int e = mlibc::sysdep_or_enosys<AioReturn>(cb, &result); e) {
		errno = e;
		return -1;
	}
	return result;
}

int aio_error(const struct aiocb *cb) {
	int result = 0;
	if (int e = mlibc::sysdep_or_enosys<AioError>(cb, &result); e) {
		errno = e;
		return -1;
	}
	return result;
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
