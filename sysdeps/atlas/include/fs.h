#pragma once

#include <abi-bits/dev_t.h>
#include <abi-bits/stat.h>
#include <stddef.h>
#include <stdint.h>

typedef enum vtype {
	VFS_FILE = 0x01,
	VFS_DIRECTORY = 0x02,
	VFS_CHARDEVICE = 0x03,
	VFS_BLOCKDEVICE = 0x04,
	VFS_PIPE = 0x05,
	VFS_SYMLINK = 0x06,
	VFS_INVALID_FS = 0x09,
} vtype_t;

struct vnops;
struct vfs;

typedef struct directory_entry {
	ino_t d_ino;
	off_t d_off;
	uint16_t d_reclen;
	vtype_t d_type;
	char d_name[256];
} DirectoryEntry;

typedef struct vattr {
	vtype_t type;
	mode_t mode;
	size_t size;
	dev_t rdev; /*! device represented by file */
} VAttr;

typedef struct vfs_node_stat {
	vtype_t type;
	ino_t inode;
	size_t filesize;
	dev_t rdev;
} __attribute__((packed)) VFSNodeStat;
