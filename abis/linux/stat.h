#ifndef _ABIBITS_STAT_H
#define _ABIBITS_STAT_H

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <bits/off_t.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/dev_t.h>
#include <abi-bits/ino_t.h>
#include <abi-bits/blksize_t.h>
#include <abi-bits/blkcnt_t.h>
#include <abi-bits/nlink_t.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>

#define S_IFMT 0x0F000
#define S_IFBLK 0x06000
#define S_IFCHR 0x02000
#define S_IFIFO 0x01000
#define S_IFREG 0x08000
#define S_IFDIR 0x04000
#define S_IFLNK 0x0A000
#define S_IFSOCK 0x0C000

#define S_IRWXU 0700
#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRWXG 070
#define S_IRGRP 040
#define S_IWGRP 020
#define S_IXGRP 010
#define S_IRWXO 07
#define S_IROTH 04
#define S_IWOTH 02
#define S_IXOTH 01
#define S_ISUID 04000
#define S_ISGID 02000
#define S_ISVTX 01000

#define S_IREAD  S_IRUSR
#define S_IWRITE S_IWUSR
#define S_IEXEC  S_IXUSR

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__x86_64__)

struct stat {
	dev_t st_dev;
	ino_t st_ino;
	nlink_t st_nlink;
	mode_t st_mode;
	uid_t st_uid;
	gid_t st_gid;
	unsigned int __pad0;
	dev_t st_rdev;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long __unused[3];
};

#elif (defined(__riscv) && __riscv_xlen == 64) || defined (__aarch64__) || defined(__loongarch64)

struct stat {
	dev_t st_dev;
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	dev_t __pad1;
	off_t st_size;
	blksize_t st_blksize;
	int __pad2;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	int __pad3[2];
};

#elif defined(__i386__)

struct stat {
	dev_t st_dev;
	int __st_dev_padding;
	long __st_ino_truncated;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	int __st_rdev_padding;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct {
		long tv_sec;
		long tv_nsec;
	} __st_atim32, __st_mtim32, __st_ctim32;
	ino_t st_ino;

	/* These fields are not in the ABI. Their values are */
	/* copied from __st_atim32, __st_mtim32, __st_ctim32 */
	/* accordingly. */

	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
};
#elif defined (__m68k__)

struct stat {
	dev_t st_dev;
	unsigned char __st_dev_padding[2];
	unsigned long __st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	unsigned char __st_rdev_padding;
	long long st_size; /* TODO: off64_t? */
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	ino_t st_ino;
};

#endif

#define stat64 stat

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_STAT_H */
