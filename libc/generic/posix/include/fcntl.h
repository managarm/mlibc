
#ifndef _FCNTL_H
#define _FCNTL_H

#include <mlibc/mode_t.h>
#include <mlibc/off_t.h>
#include <mlibc/pid_t.h>
#include <mlibc/seek.h>

#ifdef __cplusplus
extern "C" {
#endif

// constants for fcntl()'s command argument
#define F_DUPFD 1
#define F_DUPFD_CLOEXEC 2
#define F_GETFD 3
#define F_SETFD 4
#define F_GETFL 5
#define F_SETFL 6
#define F_GETLK 7
#define F_SETLK 8
#define F_SETLKW 9
#define F_GETOWN 10
#define F_SETOWN 11

// constants for struct flock's l_type member
#define F_RDLCK 1
#define F_UNLCK 2
#define F_WRLCK 3

// constants for fcntl()'s additional argument of F_GETFD and F_SETFD
#define FD_CLOEXEC 1

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

#define POSIX_FADV_NORMAL 1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_NOREUSE 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_WILLNEED 5
#define POSIX_FADV_RANDOM 6

struct flock {
	short l_type;
	short l_whence;
	off_t l_start;
	off_t l_len;
	pid_t l_pid;
};

int creat(const char *, mode_t);
int fcntl(int fd, int command, ...);
int open(const char *path, int flags, ...);
int openat(int, const char *, int, ...);
int posix_fadvise(int, off_t, off_t, int);
int posix_fallocate(int, off_t, off_t);

#ifdef __cplusplus
}
#endif

#endif // _FCNTL_H

