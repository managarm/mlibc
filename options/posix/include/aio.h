#ifndef _AIO_H
#define _AIO_H

#include <abi-bits/aio.h>
#include <abi-bits/signal.h>
#include <bits/ansi/timespec.h>
#include <bits/off_t.h>
#include <bits/posix/pthread_attr_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>

#ifdef __cplusplus
extern "C" {
#endif

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

