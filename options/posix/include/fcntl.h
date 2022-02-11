
#ifndef _FCNTL_H
#define _FCNTL_H

#include <abi-bits/abi.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/seek-whence.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/pid_t.h>
#include <bits//off_t.h>

#ifdef __cplusplus
extern "C" {
#endif

// reserve 3 bits for the access mode
#define O_ACCMODE __MLIBC_O_ACCMODE
#define O_EXEC __MLIBC_O_EXEC
#define O_RDONLY __MLIBC_O_RDONLY
#define O_RDWR __MLIBC_O_RDWR
#define O_SEARCH __MLIBC_O_SEARCH
#define O_WRONLY __MLIBC_O_WRONLY
// all remaining flags get their own bit
#define O_APPEND __MLIBC_O_APPEND
#define O_CREAT __MLIBC_O_CREAT
#define O_DIRECTORY __MLIBC_O_DIRECTORY
#define O_EXCL __MLIBC_O_EXCL
#define O_NOCTTY __MLIBC_O_NOCTTY
#define O_NOFOLLOW __MLIBC_O_NOFOLLOW
#define O_TRUNC __MLIBC_O_TRUNC
#define O_NONBLOCK __MLIBC_O_NONBLOCK
#define O_NDELAY __MLIBC_O_NONBLOCK
#define O_DSYNC __MLIBC_O_DSYNC
#define O_RSYNC __MLIBC_O_RSYNC
#define O_SYNC __MLIBC_O_SYNC
#define O_CLOEXEC __MLIBC_O_CLOEXEC
#define O_PATH __MLIBC_O_PATH
#define O_LARGEFILE __MLIBC_O_LARGEFILE
#define O_NOATIME __MLIBC_O_NOATIME

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

// This is a linux extension

struct file_handle {
        unsigned int handle_bytes;
        int handle_type;
        unsigned char f_handle[0];
};

int name_to_handle_at(int, const char *, struct file_handle *, int *, int);
int open_by_handle_at(int, struct file_handle *, int);

#ifdef __cplusplus
}
#endif

#endif // _FCNTL_H

