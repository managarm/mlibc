#ifndef _ABIBITS_STATVFS_H
#define _ABIBITS_STATVFS_H

#include <abi-bits/fsblkcnt_t.h>
#include <abi-bits/fsfilcnt_t.h>

#define ST_RDONLY 1
#define ST_NOSUID 2

// On Linux, this struct is not directly used by the kernel.
struct statvfs {
	unsigned long f_bsize;
	unsigned long f_frsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_bavail;

	fsfilcnt_t f_files;
	fsfilcnt_t f_ffree;
	fsfilcnt_t f_favail;

	unsigned long f_fsid;
	unsigned long f_flag;
	unsigned long f_namemax;
};

#endif /* _ABIBITS_STATVFS_H */

