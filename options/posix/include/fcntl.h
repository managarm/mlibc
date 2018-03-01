
#ifndef _FCNTL_H
#define _FCNTL_H

#include <bits/posix/mode_t.h>
#include <bits/posix/off_t.h>
#include <bits/posix/pid_t.h>
#include <bits/ansi/seek.h>
#include <bits/abi.h>

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
#define O_DSYNC __MLIBC_O_DSYNC
#define O_RSYNC __MLIBC_O_RSYNC
#define O_SYNC __MLIBC_O_SYNC
#define O_CLOEXEC __MLIBC_O_CLOEXEC

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

#define AT_EMPTY_PATH 1
#define AT_SYMLINK_FOLLOW 2
#define AT_SYMLINK_NOFOLLOW 4
#define AT_REMOVEDIR 8

#define AT_FDCWD -100

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

