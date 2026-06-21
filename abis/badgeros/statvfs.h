#ifndef _ABIBITS_STATVFS_H
#define _ABIBITS_STATVFS_H

#include <mlibc-config.h>

#include <abi-bits/fsblkcnt_t.h>
#include <abi-bits/fsfilcnt_t.h>

#define ST_RDONLY 1
#define ST_NOSUID 2
#define ST_NODEV 4
#define ST_NOEXEC 8
#define ST_SYNCHRONOUS 16
#define ST_MANDLOCK 64
#define ST_WRITE 128
#define ST_APPEND 256
#define ST_IMMUTABLE 512
#define ST_NOATIME 1024
#define ST_NODIRATIME 2048
#define ST_WAIT 4096
#define ST_NOWAIT 8192

#define MNT_RDONLY ST_RDONLY
#define MNT_NOSUID ST_NOSUID
#define MNT_NODEV ST_NODEV
#define MNT_NOEXEC ST_NOEXEC
#define MNT_SYNCHRONOUS ST_SYNCHRONOUS
#define MNT_MANDLOCK ST_MANDLOCK
#define MNT_WRITE ST_WRITE
#define MNT_APPEND ST_APPEND
#define MNT_IMMUTABLE ST_IMMUTABLE
#define MNT_NOATIME ST_NOATIME
#define MNT_NODIRATIME ST_NODIRATIME
#define MNT_WAIT ST_WAIT
#define MNT_NOWAIT ST_NOWAIT

#define _VFS_NAMELEN 32
/* BadgerOS: Equal to PATH_MAX. */
#define _VFS_MNAMELEN 4096

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

	char f_fstypename[_VFS_NAMELEN];
	char f_mntonname[_VFS_MNAMELEN];
	char f_mntfromname[_VFS_NAMELEN];
};

#endif /* _ABIBITS_STATVFS_H */
