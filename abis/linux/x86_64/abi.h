#ifndef _ABIBITS_ABI_H
#define _ABIBITS_ABI_H

#define __MLIBC_LINUX_O_PATH 010000000

#define __MLIBC_O_ACCMODE (03 | __MLIBC_LINUX_O_PATH)
#define __MLIBC_O_RDONLY   00
#define __MLIBC_O_WRONLY   01
#define __MLIBC_O_RDWR     02

#define __MLIBC_O_CREAT         0100
#define __MLIBC_O_EXCL          0200
#define __MLIBC_O_NOCTTY        0400
#define __MLIBC_O_TRUNC        01000
#define __MLIBC_O_APPEND       02000
#define __MLIBC_O_NONBLOCK     04000
#define __MLIBC_O_DSYNC       010000
#define __MLIBC_O_DIRECTORY  0200000
#define __MLIBC_O_NOFOLLOW   0400000
#define __MLIBC_O_CLOEXEC   02000000
#define __MLIBC_O_SYNC      04010000
#define __MLIBC_O_RSYNC     04010000

#define __MLIBC_O_EXEC __MLIBC_LINUX_O_PATH
#define __MLIBC_O_SEARCH __MLIBC_LINUX_O_PATH

#endif // _ABIBITS_ABI_H
