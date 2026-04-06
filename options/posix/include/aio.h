#ifndef _AIO_H
#define _AIO_H

#include <abi-bits/signal.h>
#include <bits/ansi/timespec.h>
#include <bits/off_t.h>
#include <bits/posix/pthread_attr_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>

#define AIO_CANCELED 0
#define AIO_NOTCANCELED 1
#define AIO_ALLDONE 2

#define LIO_READ 0
#define LIO_WRITE 1
#define LIO_NOP 2

#define LIO_WAIT 0
#define LIO_NOWAIT 1

#ifdef __cplusplus
extern "C" {
#endif

struct aiocb {
	int aio_fildes;
	off_t aio_offset;
	volatile void *aio_buf;
	size_t aio_nbytes;
	int aio_reqprio;
	struct sigevent aio_sigevent;
	int aio_lio_opcode;
	int __state;
	ssize_t __res;
	int __err;
	void *__ctx;
}
#if defined(__m68k__)
__attribute__((aligned(4)))
#endif /* defined(__m68k__) */
;

#ifndef __MLIBC_ABI_ONLY

int aio_read(struct aiocb *__cb);
int aio_write(struct aiocb *__cb);
int aio_fsync(int __op, struct aiocb *__cb);

int lio_listio(int __mode, struct aiocb *__restrict const __list[], int __n, struct sigevent *__restrict __sevp);

ssize_t aio_return(struct aiocb *__cb);
int aio_suspend(const struct aiocb *const __list[], int __nent, const struct timespec *__timeout);
int aio_error(const struct aiocb *__cb);
int aio_cancel(int __fildes, struct aiocb *__cb);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _AIO_H */

