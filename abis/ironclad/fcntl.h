#ifndef _ABIBITS_FCNTL_H
#define _ABIBITS_FCNTL_H

// reserve 3 bits for the access mode
#define O_ACCMODE 0x0007
#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR   3
#define O_SEARCH 4
#define O_EXEC   7

// This flags are stubs.
#define O_APPEND 0x00008
#define O_CREAT 0x00010
#define O_DIRECTORY 0x00020
#define O_EXCL 0x00040
#define O_NOCTTY 0x00080
#define O_NOFOLLOW 0x00100
#define O_TRUNC 0x00200
#define O_NONBLOCK 0x00400
#define O_DSYNC 0x00800
#define O_RSYNC 0x01000
#define O_SYNC 0x02000
#define O_CLOEXEC 0x04000
#define O_PATH 0x08000
#define O_LARGEFILE 0x10000
#define O_NOATIME 0x20000
#define O_TMPFILE 0x40000

// The constants below are copied from abis/mlibc/fcntl.h.
#define F_DUPFD 1
#define F_DUPFD_CLOEXEC 2
#define F_GETFD 3
#define F_SETFD 4
#define F_GETFL 5
#define F_SETFL 6
#define F_GETLK 7
#define F_SETLK 8
#define F_SETLKW 9
#define F_GETOWN 10
#define F_SETOWN 11

#define F_RDLCK 1
#define F_UNLCK 2
#define F_WRLCK 3

#define FD_CLOEXEC 1

#define F_SEAL_SHRINK 0x0002
#define F_SEAL_GROW   0x0004
#define F_SEAL_WRITE  0x0008
#define F_SEAL_SEAL   0x0010
#define F_ADD_SEALS   1033
#define F_GET_SEALS   1034

#define AT_EMPTY_PATH 1
#define AT_SYMLINK_FOLLOW 2
#define AT_SYMLINK_NOFOLLOW 4
#define AT_REMOVEDIR 8
#define AT_EACCESS 512

#define AT_FDCWD -100

#endif // _ABIBITS_FCNTL_H
