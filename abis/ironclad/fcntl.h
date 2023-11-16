#ifndef _ABIBITS_FCNTL_H
#define _ABIBITS_FCNTL_H

// Flags supported by the kernel.
#define O_ACCMODE   0b000011
#define O_RDONLY    00000001
#define O_WRONLY    0b000010
#define O_RDWR      0b000011
#define O_APPEND    0b000100
#define O_CLOEXEC   0b001000
#define O_NOFOLLOW  0b010000
#define O_NONBLOCK  0b100000

// Flags emulated by userland, we just have to make sure they dont overlap with
// kernel flags.
#define O_CREAT 0b0010000000
#define O_EXCL  0b0100000000
#define O_TRUNC 0b1000000000

// Stubbed flags, the value really doesnt matter as long as they dont overlap
// with usable ones.
// Implemented here as some software needs them to compile.
#define O_SEARCH    0b000000000010000000000
#define O_EXEC      0b000000000100000000000
#define O_NOCTTY    0b000000001000000000000
#define O_DSYNC     0b000000010000000000000
#define O_RSYNC     0b000000100000000000000
#define O_SYNC      0b000001000000000000000
#define O_PATH      0b000010000000000000000
#define O_DIRECTORY 0b000100000000000000000
#define O_LARGEFILE 0b001000000000000000000
#define O_NOATIME   0b010000000000000000000
#define O_TMPFILE   0b100000000000000000000

// Fcntl flags.
#define FD_CLOEXEC   1
#define F_DUPFD 1
#define F_DUPFD_CLOEXEC 2
#define F_GETFD      3
#define F_SETFD      4
#define F_GETFL      5
#define F_SETFL      6
#define F_GETPIPE_SZ 7
#define F_SETPIPE_SZ 8
#define F_GETLK      9
#define F_SETLK      10
#define F_SETLKW     11

#define F_RDLCK 1
#define F_UNLCK 2
#define F_WRLCK 3

// Stubbed fcntl flags.
#define F_GETOWN 10
#define F_SETOWN 11

#define F_SEAL_SHRINK 0x0002
#define F_SEAL_GROW   0x0004
#define F_SEAL_WRITE  0x0008
#define F_SEAL_SEAL   0x0010
#define F_ADD_SEALS   1033
#define F_GET_SEALS   1034

// At flags.
#define AT_REMOVEDIR        500
#define AT_EACCESS          512
#define AT_FDCWD            0x7FFFFFFF
#define AT_EMPTY_PATH       1
#define AT_SYMLINK_FOLLOW   0
#define AT_SYMLINK_NOFOLLOW 2

#define POSIX_FADV_NORMAL 1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_NOREUSE 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_WILLNEED 5
#define POSIX_FADV_RANDOM 6

#endif // _ABIBITS_FCNTL_H
