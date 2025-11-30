
#ifndef _FCNTL_H
#define _FCNTL_H

#include <mlibc-config.h>

#include <abi-bits/fcntl.h>
#include <abi-bits/seek-whence.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/pid_t.h>
#include <bits/off_t.h>
#include <bits/ssize_t.h>
#include <bits/size_t.h>

#include <sys/stat.h>
#include <unistd.h>

#if defined(_GNU_SOURCE)
#include <bits/posix/iovec.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define O_NDELAY O_NONBLOCK

/* WARNING: keep `flock` and `flock64` in sync or bad things will happen! */

struct flock {
	short l_type;
	short l_whence;
	off_t l_start;
	off_t l_len;
	pid_t l_pid;
};

#if __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE)
struct flock64 {
	short l_type;
	short l_whence;
	off_t l_start;
	off_t l_len;
	pid_t l_pid;
};
#endif /* __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE) */

#ifndef __MLIBC_ABI_ONLY

int creat(const char *__path, mode_t __mode);
int fcntl(int __fd, int __command, ...);
int open(const char *__path, int __flags, ...);
#if __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE)
int open64(const char *__path, int __flags, ...);
#endif /* __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE) */
int openat(int __dirfd, const char *__path, int __flags, ...);
int posix_fadvise(int __fd, off_t __offset, off_t __size, int __advice);
int posix_fallocate(int __fd, off_t __offset, off_t __size);

#endif /* !__MLIBC_ABI_ONLY */

/* This is a linux extension */
#if __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE)
struct file_handle {
        unsigned int handle_bytes;
        int handle_type;
        __extension__ unsigned char f_handle[0];
};
#endif /* __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE) */

#ifndef __MLIBC_ABI_ONLY

#if __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE)
int fallocate(int __fd, int __mode, off_t __offset, off_t __len);

int name_to_handle_at(int __dirfd, const char *__path, struct file_handle *__handle, int *__mount_id, int __flags);
int open_by_handle_at(int __dirfd, struct file_handle *__handle, int __flags);

ssize_t splice(int __fd_in, off_t *__off_in, int __fd_out, off_t *__off_out, size_t __len, unsigned int __flags);
ssize_t vmsplice(int __fd, const struct iovec *__iov, size_t __nr_segs, unsigned int __flags);
#endif /* __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#if __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE)
#define SPLICE_F_MOVE 1
#define SPLICE_F_NONBLOCK 2
#define SPLICE_F_MORE 4
#define SPLICE_F_GIFT 8

#define FALLOC_FL_KEEP_SIZE 1
#define FALLOC_FL_PUNCH_HOLE 2
#endif /* __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE) */

#ifdef __cplusplus
}
#endif

#endif /* _FCNTL_H */

