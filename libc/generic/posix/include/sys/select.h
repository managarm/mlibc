
#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <mlibc/time_t.h>
#include <mlibc/suseconds_t.h>
#include <mlibc/timeval.h>

#ifdef __cplusplus
extern "C" {
#endif

// FIXME: use something like uint8_t with fixed bit-size
typedef char fd_set[128];

#define FD_SETSIZE 1024

void FD_CLR(int fd, fd_set *set_ptr);
int FD_ISSET(int fd, fd_set *set_ptr);
void FD_SET(int fd, fd_set *set_ptr);
void FD_ZERO(fd_set *set_ptr);

// MISSING: pselect

int select(int, fd_set *__restrict, fd_set *__restrict, fd_set *__restrict,
		struct timeval *__restrict);

#ifdef __cplusplus
}
#endif

#endif // _SYS_SELECT_H

