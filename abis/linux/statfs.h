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

#if defined(__x86_64__) || defined(__i386__) || defined(__aarch64__) || defined(__riscv) || defined(__m68k__) || defined(__loongarch64)

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

#elif defined(__mips64)

struct statfs {
	long f_type;
#define f_fstyp f_type
	long f_bsize;
	long f_frsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_files;
	fsblkcnt_t f_ffree;
	fsblkcnt_t f_bavail;

	fsid_t f_fsid;
	long f_namelen;
	long f_flags;
	long f_spare[5];
};

struct statfs64 {
	long f_type;
	long f_bsize;
	long f_frsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_files;
	fsblkcnt_t f_ffree;
	fsblkcnt_t f_bavail;

	fsid_t f_fsid;
	long f_namelen;
	long f_flags;
	long f_spare[5];
};

#else
#error "Missing architecture specific code."
#endif

#endif /* _ABIBITS_STATFS_H */

