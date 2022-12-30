#ifndef _LINUX_FIEMAP_H
#define _LINUX_FIEMAP_H

#include <stdint.h>

struct fiemap_extent {
	uint64_t fe_logical;
	uint64_t fe_physical;
	uint64_t fe_length;
	uint64_t fe_reserved64[2];
	uint32_t fe_flags;
	uint32_t fe_reserved[3];
};

struct fiemap {
	uint64_t fm_start;
	uint64_t fm_length;
	uint32_t fm_flags;
	uint32_t fm_mapped_extents;
	uint32_t fm_extent_count;
	uint32_t fm_reserved;
	struct fiemap_extent fm_extents[];
};

#define FIEMAP_FLAG_SYNC 0x00000001
#define FIEMAP_EXTENT_LAST 0x00000001
#define FIEMAP_EXTENT_UNWRITTEN 0x00000800

#endif // _LINUX_FIEMAP_H
