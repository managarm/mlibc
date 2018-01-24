
#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/posix/fsblkcnt_t.h>
#include <bits/posix/fsfilcnt_t.h>

#define ST_RDONLY 1
#define ST_NOSUID 2

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

int statvfs(const char *__restrict, struct statvfs *__restirct);
int fstatvfs(int, struct statvfs *);

#ifdef __cplusplus
}
#endif

#endif // _SYS_STATVFS_H

