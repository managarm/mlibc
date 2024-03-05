#ifndef _ABIBITS_STATX_H
#define _ABIBITS_STATX_H

#include <bits/types.h>

struct statx_timestamp {
	__mlibc_int64 tv_sec;
	__mlibc_uint32 tv_nsec;
	__mlibc_uint32 __padding;
};

struct statx {
	__mlibc_uint32 stx_mask;

	__mlibc_uint32 stx_blksize;
	__mlibc_uint64 stx_attributes;
	__mlibc_uint32 stx_nlink;
	__mlibc_uint32 stx_uid;
	__mlibc_uint32 stx_gid;
	__mlibc_uint16 stx_mode;
	__mlibc_uint16 __padding;
	__mlibc_uint64 stx_ino;
	__mlibc_uint64 stx_size;
	__mlibc_uint64 stx_blocks;
	__mlibc_uint64 stx_attributes_mask;

	struct statx_timestamp stx_atime;
	struct statx_timestamp stx_btime;
	struct statx_timestamp stx_ctime;
	struct statx_timestamp stx_mtime;

	__mlibc_uint32 stx_rdev_major;
	__mlibc_uint32 stx_rdev_minor;
	__mlibc_uint32 stx_dev_major;
	__mlibc_uint32 stx_dev_minor;

	__mlibc_uint64 stx_mnt_id;
	__mlibc_uint32 stx_dio_mem_align;
	__mlibc_uint32 stx_dio_mem_offset;

	__mlibc_uint64 __padding1[12];
};

#endif
