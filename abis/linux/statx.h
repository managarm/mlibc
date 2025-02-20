#ifndef _ABIBITS_STATX_H
#define _ABIBITS_STATX_H

#include <mlibc-config.h>

#if !__MLIBC_LINUX_OPTION
#  error "statx() is inherently Linux specific. Enable the Linux option or do not use this header."
#endif /* !__MLIBC_LINUX_OPTION */

#include <bits/types.h>

struct statx_timestamp {
	__mlibc_int64 tv_sec;
	__mlibc_uint32 tv_nsec;
	__mlibc_uint32 __padding;
};

#define STATX_TYPE 0x1
#define STATX_MODE 0x2
#define STATX_NLINK 0x4
#define STATX_UID 0x8
#define STATX_GID 0x10
#define STATX_ATIME 0x20
#define STATX_MTIME 0x40
#define STATX_CTIME 0x80
#define STATX_INO 0x100
#define STATX_SIZE 0x200
#define STATX_BLOCKS 0x400
#define STATX_BASIC_STATS 0x7ff
#define STATX_BTIME 0x800
#define STATX_MNT_ID 0x1000
#define STATX_DIOALIGN 0x2000
#define STATX_ALL 0xfff

#define STATX_ATTR_COMPRESSED 0x4
#define STATX_ATTR_IMMUTABLE 0x10
#define STATX_ATTR_APPEND 0x20
#define STATX_ATTR_NODUMP 0x40
#define STATX_ATTR_ENCRYPTED 0x800
#define STATX_ATTR_AUTOMOUNT 0x1000
#define STATX_ATTR_MOUNT_ROOT 0x2000
#define STATX_ATTR_VERITY 0x100000
#define STATX_ATTR_DAX 0x200000

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
	__mlibc_uint32 stx_dio_offset_align;

	__mlibc_uint64 __padding1[12];
};

#endif
