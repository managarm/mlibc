/**
 * The contents of this file are sourced from a header provided
 * with a Linux kernel version 5-lts. It was originally distributed
 * under GPL-2.0 or a BSD-2-clause license.*/

#ifndef _LINUX_FUSE_H
#define _LINUX_FUSE_H

#include <stdint.h>

/*
 * Version negotiation:
 *
 * Both the kernel and userspace send the version they support in the
 * INIT request and reply respectively.
 *
 * If the major versions match then both shall use the smallest
 * of the two minor versions for communication.
 *
 * If the kernel supports a larger major version, then userspace shall
 * reply with the major version it supports, ignore the rest of the
 * INIT message and expect a new INIT message from the kernel with a
 * matching major version.
 *
 * If the library supports a larger major version, then it shall fall
 * back to the major protocol version sent by the kernel for
 * communication and reply with that major version (and an arbitrary
 * supported minor version).
 */

/** Version number of this interface */
#define FUSE_KERNEL_VERSION 7

/** Minor version number of this interface */
#define FUSE_KERNEL_MINOR_VERSION 36

/** The node ID of the root inode */
#define FUSE_ROOT_ID 1

/* Make sure all structures are padded to 64bit boundary, so 32bit
   userspace works under 64bit kernels */

struct fuse_attr {
    uint64_t    ino;
    uint64_t    size;
    uint64_t    blocks;
    uint64_t    atime;
    uint64_t    mtime;
    uint64_t    ctime;
    uint32_t    atimensec;
    uint32_t    mtimensec;
    uint32_t    ctimensec;
    uint32_t    mode;
    uint32_t    nlink;
    uint32_t    uid;
    uint32_t    gid;
    uint32_t    rdev;
    uint32_t    blksize;
    uint32_t    flags;
};

struct fuse_kstatfs {
    uint64_t    blocks;
    uint64_t    bfree;
    uint64_t    bavail;
    uint64_t    files;
    uint64_t    ffree;
    uint32_t    bsize;
    uint32_t    namelen;
    uint32_t    frsize;
    uint32_t    padding;
    uint32_t    spare[6];
};

struct fuse_file_lock {
    uint64_t    start;
    uint64_t    end;
    uint32_t    type;
    uint32_t    pid; /* tgid */
};

/**
 * Bitmasks for fuse_setattr_in.valid
 */
#define FATTR_MODE  (1 << 0)
#define FATTR_UID   (1 << 1)
#define FATTR_GID   (1 << 2)
#define FATTR_SIZE  (1 << 3)
#define FATTR_ATIME (1 << 4)
#define FATTR_MTIME (1 << 5)
#define FATTR_FH    (1 << 6)
#define FATTR_ATIME_NOW (1 << 7)
#define FATTR_MTIME_NOW (1 << 8)
#define FATTR_LOCKOWNER (1 << 9)
#define FATTR_CTIME (1 << 10)
#define FATTR_KILL_SUIDGID  (1 << 11)

/**
 * Flags returned by the OPEN request
 *
 * FOPEN_DIRECT_IO: bypass page cache for this open file
 * FOPEN_KEEP_CACHE: don't invalidate the data cache on open
 * FOPEN_NONSEEKABLE: the file is not seekable
 * FOPEN_CACHE_DIR: allow caching this directory
 * FOPEN_STREAM: the file is stream-like (no file position at all)
 * FOPEN_NOFLUSH: don't flush data cache on close (unless FUSE_WRITEBACK_CACHE)
 */
#define FOPEN_DIRECT_IO     (1 << 0)
#define FOPEN_KEEP_CACHE    (1 << 1)
#define FOPEN_NONSEEKABLE   (1 << 2)
#define FOPEN_CACHE_DIR     (1 << 3)
#define FOPEN_STREAM        (1 << 4)
#define FOPEN_NOFLUSH       (1 << 5)

/**
 * INIT request/reply flags
 *
 * FUSE_ASYNC_READ: asynchronous read requests
 * FUSE_POSIX_LOCKS: remote locking for POSIX file locks
 * FUSE_FILE_OPS: kernel sends file handle for fstat, etc... (not yet supported)
 * FUSE_ATOMIC_O_TRUNC: handles the O_TRUNC open flag in the filesystem
 * FUSE_EXPORT_SUPPORT: filesystem handles lookups of "." and ".."
 * FUSE_BIG_WRITES: filesystem can handle write size larger than 4kB
 * FUSE_DONT_MASK: don't apply umask to file mode on create operations
 * FUSE_SPLICE_WRITE: kernel supports splice write on the device
 * FUSE_SPLICE_MOVE: kernel supports splice move on the device
 * FUSE_SPLICE_READ: kernel supports splice read on the device
 * FUSE_FLOCK_LOCKS: remote locking for BSD style file locks
 * FUSE_HAS_IOCTL_DIR: kernel supports ioctl on directories
 * FUSE_AUTO_INVAL_DATA: automatically invalidate cached pages
 * FUSE_DO_READDIRPLUS: do READDIRPLUS (READDIR+LOOKUP in one)
 * FUSE_READDIRPLUS_AUTO: adaptive readdirplus
 * FUSE_ASYNC_DIO: asynchronous direct I/O submission
 * FUSE_WRITEBACK_CACHE: use writeback cache for buffered writes
 * FUSE_NO_OPEN_SUPPORT: kernel supports zero-message opens
 * FUSE_PARALLEL_DIROPS: allow parallel lookups and readdir
 * FUSE_HANDLE_KILLPRIV: fs handles killing suid/sgid/cap on write/chown/trunc
 * FUSE_POSIX_ACL: filesystem supports posix acls
 * FUSE_ABORT_ERROR: reading the device after abort returns ECONNABORTED
 * FUSE_MAX_PAGES: init_out.max_pages contains the max number of req pages
 * FUSE_CACHE_SYMLINKS: cache READLINK responses
 * FUSE_NO_OPENDIR_SUPPORT: kernel supports zero-message opendir
 * FUSE_EXPLICIT_INVAL_DATA: only invalidate cached pages on explicit request
 * FUSE_MAP_ALIGNMENT: init_out.map_alignment contains log2(byte alignment) for
 *             foffset and moffset fields in struct
 *             fuse_setupmapping_out and fuse_removemapping_one.
 * FUSE_SUBMOUNTS: kernel supports auto-mounting directory submounts
 * FUSE_HANDLE_KILLPRIV_V2: fs kills suid/sgid/cap on write/chown/trunc.
 *          Upon write/truncate suid/sgid is only killed if caller
 *          does not have CAP_FSETID. Additionally upon
 *          write/truncate sgid is killed only if file has group
 *          execute permission. (Same as Linux VFS behavior).
 * FUSE_SETXATTR_EXT:   Server supports extended struct fuse_setxattr_in
 * FUSE_INIT_EXT: extended fuse_init_in request
 * FUSE_INIT_RESERVED: reserved, do not use
 * FUSE_SECURITY_CTX:   add security context to create, mkdir, symlink, and
 *          mknod
 * FUSE_HAS_INODE_DAX:  use per inode DAX
 */
#define FUSE_ASYNC_READ     (1 << 0)
#define FUSE_POSIX_LOCKS    (1 << 1)
#define FUSE_FILE_OPS       (1 << 2)
#define FUSE_ATOMIC_O_TRUNC (1 << 3)
#define FUSE_EXPORT_SUPPORT (1 << 4)
#define FUSE_BIG_WRITES     (1 << 5)
#define FUSE_DONT_MASK      (1 << 6)
#define FUSE_SPLICE_WRITE   (1 << 7)
#define FUSE_SPLICE_MOVE    (1 << 8)
#define FUSE_SPLICE_READ    (1 << 9)
#define FUSE_FLOCK_LOCKS    (1 << 10)
#define FUSE_HAS_IOCTL_DIR  (1 << 11)
#define FUSE_AUTO_INVAL_DATA    (1 << 12)
#define FUSE_DO_READDIRPLUS (1 << 13)
#define FUSE_READDIRPLUS_AUTO   (1 << 14)
#define FUSE_ASYNC_DIO      (1 << 15)
#define FUSE_WRITEBACK_CACHE    (1 << 16)
#define FUSE_NO_OPEN_SUPPORT    (1 << 17)
#define FUSE_PARALLEL_DIROPS    (1 << 18)
#define FUSE_HANDLE_KILLPRIV    (1 << 19)
#define FUSE_POSIX_ACL      (1 << 20)
#define FUSE_ABORT_ERROR    (1 << 21)
#define FUSE_MAX_PAGES      (1 << 22)
#define FUSE_CACHE_SYMLINKS (1 << 23)
#define FUSE_NO_OPENDIR_SUPPORT (1 << 24)
#define FUSE_EXPLICIT_INVAL_DATA (1 << 25)
#define FUSE_MAP_ALIGNMENT  (1 << 26)
#define FUSE_SUBMOUNTS      (1 << 27)
#define FUSE_HANDLE_KILLPRIV_V2 (1 << 28)
#define FUSE_SETXATTR_EXT   (1 << 29)
#define FUSE_INIT_EXT       (1 << 30)
#define FUSE_INIT_RESERVED  (1 << 31)
/* bits 32..63 get shifted down 32 bits into the flags2 field */
#define FUSE_SECURITY_CTX   (1ULL << 32)
#define FUSE_HAS_INODE_DAX  (1ULL << 33)

/**
 * CUSE INIT request/reply flags
 *
 * CUSE_UNRESTRICTED_IOCTL:  use unrestricted ioctl
 */
#define CUSE_UNRESTRICTED_IOCTL (1 << 0)

/**
 * Release flags
 */
#define FUSE_RELEASE_FLUSH  (1 << 0)
#define FUSE_RELEASE_FLOCK_UNLOCK   (1 << 1)

/**
 * Getattr flags
 */
#define FUSE_GETATTR_FH     (1 << 0)

/**
 * Lock flags
 */
#define FUSE_LK_FLOCK       (1 << 0)

/**
 * WRITE flags
 *
 * FUSE_WRITE_CACHE: delayed write from page cache, file handle is guessed
 * FUSE_WRITE_LOCKOWNER: lock_owner field is valid
 * FUSE_WRITE_KILL_SUIDGID: kill suid and sgid bits
 */
#define FUSE_WRITE_CACHE    (1 << 0)
#define FUSE_WRITE_LOCKOWNER    (1 << 1)
#define FUSE_WRITE_KILL_SUIDGID (1 << 2)

/* Obsolete alias; this flag implies killing suid/sgid only. */
#define FUSE_WRITE_KILL_PRIV    FUSE_WRITE_KILL_SUIDGID

/**
 * Read flags
 */
#define FUSE_READ_LOCKOWNER (1 << 1)

/**
 * Ioctl flags
 *
 * FUSE_IOCTL_COMPAT: 32bit compat ioctl on 64bit machine
 * FUSE_IOCTL_UNRESTRICTED: not restricted to well-formed ioctls, retry allowed
 * FUSE_IOCTL_RETRY: retry with new iovecs
 * FUSE_IOCTL_32BIT: 32bit ioctl
 * FUSE_IOCTL_DIR: is a directory
 * FUSE_IOCTL_COMPAT_X32: x32 compat ioctl on 64bit machine (64bit time_t)
 *
 * FUSE_IOCTL_MAX_IOV: maximum of in_iovecs + out_iovecs
 */
#define FUSE_IOCTL_COMPAT   (1 << 0)
#define FUSE_IOCTL_UNRESTRICTED (1 << 1)
#define FUSE_IOCTL_RETRY    (1 << 2)
#define FUSE_IOCTL_32BIT    (1 << 3)
#define FUSE_IOCTL_DIR      (1 << 4)
#define FUSE_IOCTL_COMPAT_X32   (1 << 5)

#define FUSE_IOCTL_MAX_IOV  256

/**
 * Poll flags
 *
 * FUSE_POLL_SCHEDULE_NOTIFY: request poll notify
 */
#define FUSE_POLL_SCHEDULE_NOTIFY (1 << 0)

/**
 * Fsync flags
 *
 * FUSE_FSYNC_FDATASYNC: Sync data only, not metadata
 */
#define FUSE_FSYNC_FDATASYNC    (1 << 0)

/**
 * fuse_attr flags
 *
 * FUSE_ATTR_SUBMOUNT: Object is a submount root
 * FUSE_ATTR_DAX: Enable DAX for this file in per inode DAX mode
 */
#define FUSE_ATTR_SUBMOUNT      (1 << 0)
#define FUSE_ATTR_DAX       (1 << 1)

/**
 * Open flags
 * FUSE_OPEN_KILL_SUIDGID: Kill suid and sgid if executable
 */
#define FUSE_OPEN_KILL_SUIDGID  (1 << 0)

/**
 * setxattr flags
 * FUSE_SETXATTR_ACL_KILL_SGID: Clear SGID when system.posix_acl_access is set
 */
#define FUSE_SETXATTR_ACL_KILL_SGID (1 << 0)

enum fuse_opcode {
    FUSE_LOOKUP     = 1,
    FUSE_FORGET     = 2,  /* no reply */
    FUSE_GETATTR        = 3,
    FUSE_SETATTR        = 4,
    FUSE_READLINK       = 5,
    FUSE_SYMLINK        = 6,
    FUSE_MKNOD      = 8,
    FUSE_MKDIR      = 9,
    FUSE_UNLINK     = 10,
    FUSE_RMDIR      = 11,
    FUSE_RENAME     = 12,
    FUSE_LINK       = 13,
    FUSE_OPEN       = 14,
    FUSE_READ       = 15,
    FUSE_WRITE      = 16,
    FUSE_STATFS     = 17,
    FUSE_RELEASE        = 18,
    FUSE_FSYNC      = 20,
    FUSE_SETXATTR       = 21,
    FUSE_GETXATTR       = 22,
    FUSE_LISTXATTR      = 23,
    FUSE_REMOVEXATTR    = 24,
    FUSE_FLUSH      = 25,
    FUSE_INIT       = 26,
    FUSE_OPENDIR        = 27,
    FUSE_READDIR        = 28,
    FUSE_RELEASEDIR     = 29,
    FUSE_FSYNCDIR       = 30,
    FUSE_GETLK      = 31,
    FUSE_SETLK      = 32,
    FUSE_SETLKW     = 33,
    FUSE_ACCESS     = 34,
    FUSE_CREATE     = 35,
    FUSE_INTERRUPT      = 36,
    FUSE_BMAP       = 37,
    FUSE_DESTROY        = 38,
    FUSE_IOCTL      = 39,
    FUSE_POLL       = 40,
    FUSE_NOTIFY_REPLY   = 41,
    FUSE_BATCH_FORGET   = 42,
    FUSE_FALLOCATE      = 43,
    FUSE_READDIRPLUS    = 44,
    FUSE_RENAME2        = 45,
    FUSE_LSEEK      = 46,
    FUSE_COPY_FILE_RANGE    = 47,
    FUSE_SETUPMAPPING   = 48,
    FUSE_REMOVEMAPPING  = 49,
    FUSE_SYNCFS     = 50,

    /* CUSE specific operations */
    CUSE_INIT       = 4096,

    /* Reserved opcodes: helpful to detect structure endian-ness */
    CUSE_INIT_BSWAP_RESERVED    = 1048576,  /* CUSE_INIT << 8 */
    FUSE_INIT_BSWAP_RESERVED    = 436207616,    /* FUSE_INIT << 24 */
};

enum fuse_notify_code {
    FUSE_NOTIFY_POLL   = 1,
    FUSE_NOTIFY_INVAL_INODE = 2,
    FUSE_NOTIFY_INVAL_ENTRY = 3,
    FUSE_NOTIFY_STORE = 4,
    FUSE_NOTIFY_RETRIEVE = 5,
    FUSE_NOTIFY_DELETE = 6,
    FUSE_NOTIFY_CODE_MAX,
};

/* The read buffer is required to be at least 8k, but may be much larger */
#define FUSE_MIN_READ_BUFFER 8192

#define FUSE_COMPAT_ENTRY_OUT_SIZE 120

struct fuse_entry_out {
    uint64_t    nodeid;     /* Inode ID */
    uint64_t    generation; /* Inode generation: nodeid:gen must
                       be unique for the fs's lifetime */
    uint64_t    entry_valid;    /* Cache timeout for the name */
    uint64_t    attr_valid; /* Cache timeout for the attributes */
    uint32_t    entry_valid_nsec;
    uint32_t    attr_valid_nsec;
    struct fuse_attr attr;
};

struct fuse_forget_in {
    uint64_t    nlookup;
};

struct fuse_forget_one {
    uint64_t    nodeid;
    uint64_t    nlookup;
};

struct fuse_batch_forget_in {
    uint32_t    count;
    uint32_t    dummy;
};

struct fuse_getattr_in {
    uint32_t    getattr_flags;
    uint32_t    dummy;
    uint64_t    fh;
};

#define FUSE_COMPAT_ATTR_OUT_SIZE 96

struct fuse_attr_out {
    uint64_t    attr_valid; /* Cache timeout for the attributes */
    uint32_t    attr_valid_nsec;
    uint32_t    dummy;
    struct fuse_attr attr;
};

#define FUSE_COMPAT_MKNOD_IN_SIZE 8

struct fuse_mknod_in {
    uint32_t    mode;
    uint32_t    rdev;
    uint32_t    umask;
    uint32_t    padding;
};

struct fuse_mkdir_in {
    uint32_t    mode;
    uint32_t    umask;
};

struct fuse_rename_in {
    uint64_t    newdir;
};

struct fuse_rename2_in {
    uint64_t    newdir;
    uint32_t    flags;
    uint32_t    padding;
};

struct fuse_link_in {
    uint64_t    oldnodeid;
};

struct fuse_setattr_in {
    uint32_t    valid;
    uint32_t    padding;
    uint64_t    fh;
    uint64_t    size;
    uint64_t    lock_owner;
    uint64_t    atime;
    uint64_t    mtime;
    uint64_t    ctime;
    uint32_t    atimensec;
    uint32_t    mtimensec;
    uint32_t    ctimensec;
    uint32_t    mode;
    uint32_t    unused4;
    uint32_t    uid;
    uint32_t    gid;
    uint32_t    unused5;
};

struct fuse_open_in {
    uint32_t    flags;
    uint32_t    open_flags; /* FUSE_OPEN_... */
};

struct fuse_create_in {
    uint32_t    flags;
    uint32_t    mode;
    uint32_t    umask;
    uint32_t    open_flags; /* FUSE_OPEN_... */
};

struct fuse_open_out {
    uint64_t    fh;
    uint32_t    open_flags;
    uint32_t    padding;
};

struct fuse_release_in {
    uint64_t    fh;
    uint32_t    flags;
    uint32_t    release_flags;
    uint64_t    lock_owner;
};

struct fuse_flush_in {
    uint64_t    fh;
    uint32_t    unused;
    uint32_t    padding;
    uint64_t    lock_owner;
};

struct fuse_read_in {
    uint64_t    fh;
    uint64_t    offset;
    uint32_t    size;
    uint32_t    read_flags;
    uint64_t    lock_owner;
    uint32_t    flags;
    uint32_t    padding;
};

#define FUSE_COMPAT_WRITE_IN_SIZE 24

struct fuse_write_in {
    uint64_t    fh;
    uint64_t    offset;
    uint32_t    size;
    uint32_t    write_flags;
    uint64_t    lock_owner;
    uint32_t    flags;
    uint32_t    padding;
};

struct fuse_write_out {
    uint32_t    size;
    uint32_t    padding;
};

#define FUSE_COMPAT_STATFS_SIZE 48

struct fuse_statfs_out {
    struct fuse_kstatfs st;
};

struct fuse_fsync_in {
    uint64_t    fh;
    uint32_t    fsync_flags;
    uint32_t    padding;
};

#define FUSE_COMPAT_SETXATTR_IN_SIZE 8

struct fuse_setxattr_in {
    uint32_t    size;
    uint32_t    flags;
    uint32_t    setxattr_flags;
    uint32_t    padding;
};

struct fuse_getxattr_in {
    uint32_t    size;
    uint32_t    padding;
};

struct fuse_getxattr_out {
    uint32_t    size;
    uint32_t    padding;
};

struct fuse_lk_in {
    uint64_t    fh;
    uint64_t    owner;
    struct fuse_file_lock lk;
    uint32_t    lk_flags;
    uint32_t    padding;
};

struct fuse_lk_out {
    struct fuse_file_lock lk;
};

struct fuse_access_in {
    uint32_t    mask;
    uint32_t    padding;
};

struct fuse_init_in {
    uint32_t    major;
    uint32_t    minor;
    uint32_t    max_readahead;
    uint32_t    flags;
    uint32_t    flags2;
    uint32_t    unused[11];
};

#define FUSE_COMPAT_INIT_OUT_SIZE 8
#define FUSE_COMPAT_22_INIT_OUT_SIZE 24

struct fuse_init_out {
    uint32_t    major;
    uint32_t    minor;
    uint32_t    max_readahead;
    uint32_t    flags;
    uint16_t    max_background;
    uint16_t    congestion_threshold;
    uint32_t    max_write;
    uint32_t    time_gran;
    uint16_t    max_pages;
    uint16_t    map_alignment;
    uint32_t    flags2;
    uint32_t    unused[7];
};

#define CUSE_INIT_INFO_MAX 4096

struct cuse_init_in {
    uint32_t    major;
    uint32_t    minor;
    uint32_t    unused;
    uint32_t    flags;
};

struct cuse_init_out {
    uint32_t    major;
    uint32_t    minor;
    uint32_t    unused;
    uint32_t    flags;
    uint32_t    max_read;
    uint32_t    max_write;
    uint32_t    dev_major;      /* chardev major */
    uint32_t    dev_minor;      /* chardev minor */
    uint32_t    spare[10];
};

struct fuse_interrupt_in {
    uint64_t    unique;
};

struct fuse_bmap_in {
    uint64_t    block;
    uint32_t    blocksize;
    uint32_t    padding;
};

struct fuse_bmap_out {
    uint64_t    block;
};

struct fuse_ioctl_in {
    uint64_t    fh;
    uint32_t    flags;
    uint32_t    cmd;
    uint64_t    arg;
    uint32_t    in_size;
    uint32_t    out_size;
};

struct fuse_ioctl_iovec {
    uint64_t    base;
    uint64_t    len;
};

struct fuse_ioctl_out {
    int32_t     result;
    uint32_t    flags;
    uint32_t    in_iovs;
    uint32_t    out_iovs;
};

struct fuse_poll_in {
    uint64_t    fh;
    uint64_t    kh;
    uint32_t    flags;
    uint32_t    events;
};

struct fuse_poll_out {
    uint32_t    revents;
    uint32_t    padding;
};

struct fuse_notify_poll_wakeup_out {
    uint64_t    kh;
};

struct fuse_fallocate_in {
    uint64_t    fh;
    uint64_t    offset;
    uint64_t    length;
    uint32_t    mode;
    uint32_t    padding;
};

struct fuse_in_header {
    uint32_t    len;
    uint32_t    opcode;
    uint64_t    unique;
    uint64_t    nodeid;
    uint32_t    uid;
    uint32_t    gid;
    uint32_t    pid;
    uint32_t    padding;
};

struct fuse_out_header {
    uint32_t    len;
    int32_t     error;
    uint64_t    unique;
};

struct fuse_dirent {
    uint64_t    ino;
    uint64_t    off;
    uint32_t    namelen;
    uint32_t    type;
    char name[];
};

/* Align variable length records to 64bit boundary */
#define FUSE_REC_ALIGN(x) \
    (((x) + sizeof(uint64_t) - 1) & ~(sizeof(uint64_t) - 1))

#define FUSE_NAME_OFFSET offsetof(struct fuse_dirent, name)
#define FUSE_DIRENT_ALIGN(x) FUSE_REC_ALIGN(x)
#define FUSE_DIRENT_SIZE(d) \
    FUSE_DIRENT_ALIGN(FUSE_NAME_OFFSET + (d)->namelen)

struct fuse_direntplus {
    struct fuse_entry_out entry_out;
    struct fuse_dirent dirent;
};

#define FUSE_NAME_OFFSET_DIRENTPLUS \
    offsetof(struct fuse_direntplus, dirent.name)
#define FUSE_DIRENTPLUS_SIZE(d) \
    FUSE_DIRENT_ALIGN(FUSE_NAME_OFFSET_DIRENTPLUS + (d)->dirent.namelen)

struct fuse_notify_inval_inode_out {
    uint64_t    ino;
    int64_t     off;
    int64_t     len;
};

struct fuse_notify_inval_entry_out {
    uint64_t    parent;
    uint32_t    namelen;
    uint32_t    padding;
};

struct fuse_notify_delete_out {
    uint64_t    parent;
    uint64_t    child;
    uint32_t    namelen;
    uint32_t    padding;
};

struct fuse_notify_store_out {
    uint64_t    nodeid;
    uint64_t    offset;
    uint32_t    size;
    uint32_t    padding;
};

struct fuse_notify_retrieve_out {
    uint64_t    notify_unique;
    uint64_t    nodeid;
    uint64_t    offset;
    uint32_t    size;
    uint32_t    padding;
};

/* Matches the size of fuse_write_in */
struct fuse_notify_retrieve_in {
    uint64_t    dummy1;
    uint64_t    offset;
    uint32_t    size;
    uint32_t    dummy2;
    uint64_t    dummy3;
    uint64_t    dummy4;
};

/* Device ioctls: */
#define FUSE_DEV_IOC_MAGIC      229
#define FUSE_DEV_IOC_CLONE      _IOR(FUSE_DEV_IOC_MAGIC, 0, uint32_t)

struct fuse_lseek_in {
    uint64_t    fh;
    uint64_t    offset;
    uint32_t    whence;
    uint32_t    padding;
};

struct fuse_lseek_out {
    uint64_t    offset;
};

struct fuse_copy_file_range_in {
    uint64_t    fh_in;
    uint64_t    off_in;
    uint64_t    nodeid_out;
    uint64_t    fh_out;
    uint64_t    off_out;
    uint64_t    len;
    uint64_t    flags;
};

#define FUSE_SETUPMAPPING_FLAG_WRITE (1ull << 0)
#define FUSE_SETUPMAPPING_FLAG_READ (1ull << 1)
struct fuse_setupmapping_in {
    /* An already open handle */
    uint64_t    fh;
    /* Offset into the file to start the mapping */
    uint64_t    foffset;
    /* Length of mapping required */
    uint64_t    len;
    /* Flags, FUSE_SETUPMAPPING_FLAG_* */
    uint64_t    flags;
    /* Offset in Memory Window */
    uint64_t    moffset;
};

struct fuse_removemapping_in {
    /* number of fuse_removemapping_one follows */
    uint32_t        count;
};

struct fuse_removemapping_one {
    /* Offset into the dax window start the unmapping */
    uint64_t        moffset;
    /* Length of mapping required */
    uint64_t    len;
};

#define FUSE_REMOVEMAPPING_MAX_ENTRY   \
        (PAGE_SIZE / sizeof(struct fuse_removemapping_one))

struct fuse_syncfs_in {
    uint64_t    padding;
};

/*
 * For each security context, send fuse_secctx with size of security context
 * fuse_secctx will be followed by security context name and this in turn
 * will be followed by actual context label.
 * fuse_secctx, name, context
 */
struct fuse_secctx {
    uint32_t    size;
    uint32_t    padding;
};

/*
 * Contains the information about how many fuse_secctx structures are being
 * sent and what's the total size of all security contexts (including
 * size of fuse_secctx_header).
 *
 */
struct fuse_secctx_header {
    uint32_t    size;
    uint32_t    nr_secctx;
};

#endif /* _LINUX_FUSE_H */
