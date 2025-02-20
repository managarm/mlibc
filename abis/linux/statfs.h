#ifndef _ABIBITS_STATFS_H
#define _ABIBITS_STATFS_H

#include <mlibc-config.h>

#if !__MLIBC_LINUX_OPTION
#  error "statfs() is inherently Linux specific. Enable the Linux option or do not use this header."
#endif /* !__MLIBC_LINUX_OPTION */

#include <abi-bits/fsblkcnt_t.h>
#include <abi-bits/fsfilcnt_t.h>

typedef struct __mlibc_fsid {
	int __val[2];
} fsid_t;

/* WARNING: keep `statfs` and `statfs64` in sync or bad things will happen! */
struct statfs {
	unsigned long f_type;
	unsigned long f_bsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_bavail;
	fsfilcnt_t f_files;
	fsfilcnt_t f_ffree;
	fsid_t f_fsid;
	unsigned long f_namelen;
	unsigned long f_frsize;
	unsigned long f_flags;
	unsigned long __f_spare[4];
};

/* WARNING: keep `statfs` and `statfs64` in sync or bad things will happen! */
struct statfs64 {
	unsigned long f_type;
	unsigned long f_bsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_bavail;
	fsfilcnt_t f_files;
	fsfilcnt_t f_ffree;
	fsid_t f_fsid;
	unsigned long f_namelen;
	unsigned long f_frsize;
	unsigned long f_flags;
	unsigned long __f_spare[4];
};

#endif /* _ABIBITS_STATFS_H */

