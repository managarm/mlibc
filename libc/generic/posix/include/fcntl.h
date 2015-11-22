
#ifndef _FCNTL_H
#define _FCNTL_H

#include <mlibc/mode_t.h>
#include <mlibc/off_t.h>
#include <mlibc/pid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

// MISSING: F macros

#define FD_CLOEXEC 1

// MISSING: SEEK macros from stdio.h

// MISSING: O macros

// MISSING: AT macros

// MISSING: POSIX_FADV macros

struct flock {
	short l_type;
	short l_whence;
	off_t l_start;
	off_t l_len;
	pid_t l_pid;
};

int creat(const char *, mode_t);
int fcntl(int, int, ...);
int open(const char *, int, ...);
int openat(int, const char *, int, ...);
int posix_fadvise(int, off_t, off_t, int);
int posix_fallocate(int, off_t, off_t);

#ifdef __cplusplus
}
#endif

#endif // _FCNTL_H

