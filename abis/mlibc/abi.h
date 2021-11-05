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
#define __MLIBC_O_APPEND 0x00008
#define __MLIBC_O_CREAT 0x00010
#define __MLIBC_O_DIRECTORY 0x00020
#define __MLIBC_O_EXCL 0x00040
#define __MLIBC_O_NOCTTY 0x00080
#define __MLIBC_O_NOFOLLOW 0x00100
#define __MLIBC_O_TRUNC 0x00200
#define __MLIBC_O_NONBLOCK 0x00400
#define __MLIBC_O_DSYNC 0x00800
#define __MLIBC_O_RSYNC 0x01000
#define __MLIBC_O_SYNC 0x02000
#define __MLIBC_O_CLOEXEC 0x04000
#define __MLIBC_O_PATH 0x08000
#define __MLIBC_O_LARGEFILE 0x10000
#define __MLIBC_O_NOATIME 0x20000

#endif // _ABIBITS_ABI_H