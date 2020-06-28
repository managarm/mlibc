
#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <abi-bits/signal.h>

#include <bits/ansi/time_t.h>
#include <bits/posix/suseconds_t.h>
#include <bits/posix/timeval.h>
#include <bits/posix/fd_set.h>

#define FD_SETSIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

void FD_CLR(int fd, fd_set *);
int FD_ISSET(int fd, fd_set *);
void FD_SET(int fd, fd_set *);
void FD_ZERO(fd_set *);

#define FD_CLR FD_CLR
#define FD_ISSET FD_ISSET
#define FD_SET FD_SET
#define FD_ZERO FD_ZERO

int select(int, fd_set *__restrict, fd_set *__restrict, fd_set *__restrict,
		struct timeval *__restrict);
int pselect(int, fd_set *, fd_set *, fd_set *, const struct timespec *,
		const sigset_t *);

#ifdef __cplusplus
}
#endif

#endif // _SYS_SELECT_H

