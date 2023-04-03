
#ifndef _FCNTL_H
#define _FCNTL_H

#include <abi-bits/fcntl.h>
#include <abi-bits/seek-whence.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/pid_t.h>
#include <bits/posix/iovec.h>
#include <bits/off_t.h>
#include <bits/ssize_t.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#define O_NDELAY O_NONBLOCK

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
int fallocate(int fd, int mode, off_t offset, off_t len);
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

ssize_t splice(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned int flags);
ssize_t vmsplice(int fd, const struct iovec *iov, size_t nr_segs, unsigned int flags);

#define SPLICE_F_MOVE 1
#define SPLICE_F_NONBLOCK 2
#define SPLICE_F_MORE 4
#define SPLICE_F_GIFT 8

#define AT_NO_AUTOMOUNT 0x800

#define F_SETPIPE_SZ 1031
#define F_GETPIPE_SZ 1032

#ifdef __cplusplus
}
#endif

#endif // _FCNTL_H

