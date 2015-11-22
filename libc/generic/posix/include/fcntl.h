
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

// reserve 3 bits for the access mode
#define O_ACCMODE 0x0007
#define O_EXEC 1
#define O_RDONLY 2
#define O_RDWR 3
#define O_SEARCH 4
#define O_WRONLY 5
// all remaining flags get their own bit
#define O_APPEND 0x0008
#define O_CREAT 0x0010
#define O_DIRECTORY 0x0020
#define O_EXCL 0x0040
#define O_NOCTTY 0x0080
#define O_NOFOLLOW 0x0100
#define O_TRUNC 0x0200
#define O_NONBLOCK 0x0400
#define O_DSYNC 0x0800
#define O_RSYNC 0x1000
#define O_SYNC 0x2000
#define O_CLOEXEC 0x4000

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

