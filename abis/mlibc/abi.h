#ifndef _ABIBITS_ABI_H
#define _ABIBITS_ABI_H

// reserve 3 bits for the access mode
#define __MLIBC_O_ACCMODE 0x0007
#define __MLIBC_O_EXEC 1
#define __MLIBC_O_RDONLY 2
#define __MLIBC_O_RDWR 3
#define __MLIBC_O_SEARCH 4
#define __MLIBC_O_WRONLY 5
// all remaining flags get their own bit
#define __MLIBC_O_APPEND 0x0008
#define __MLIBC_O_CREAT 0x0010
#define __MLIBC_O_DIRECTORY 0x0020
#define __MLIBC_O_EXCL 0x0040
#define __MLIBC_O_NOCTTY 0x0080
#define __MLIBC_O_NOFOLLOW 0x0100
#define __MLIBC_O_TRUNC 0x0200
#define __MLIBC_O_NONBLOCK 0x0400
#define __MLIBC_O_DSYNC 0x0800
#define __MLIBC_O_RSYNC 0x1000
#define __MLIBC_O_SYNC 0x2000
#define __MLIBC_O_CLOEXEC 0x4000
#define __MLIBC_O_PATH 0x8000

#endif // _ABIBITS_ABI_H