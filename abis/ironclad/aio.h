#ifndef _ABIBITS_AIO_H
#define _ABIBITS_AIO_H

#include <abi-bits/sigevent.h>
#include <bits/off_t.h>
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

#endif /* _ABIBITS_AIO_H */
