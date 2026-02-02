#ifndef _ABIBITS_FCNTL_H
#define _ABIBITS_FCNTL_H

#include <mlibc-config.h>
#include <abi-bits/pid_t.h>

// #define O_PATH

#define O_ACCMODE  0x03
// Note: If neither O_RDONLY nor O_WRONLY are specified, the kernel assumes read access was requested.
#define O_RDONLY   0x00000001
#define O_WRONLY   0x00000002
#define O_RDWR     0x00000003

#define O_CREAT    0x00000040
#define O_EXCL     0x00000080
#define O_NOCTTY   0x01000000
#define O_TRUNC    0x00000100
#define O_APPEND   0x00000004
#define O_NONBLOCK 0x00000200
#define O_CLOEXEC  0x00010000
// #define O_NOATIME

// #define O_DIRECT
#define O_DIRECTORY 0x00000010
#define O_NOFOLLOW  0x00000020

#define O_TMPFILE   0x00000400

// #define O_EXEC O_PATH
// #define O_SEARCH O_PATH
#define O_TTY_INIT 0

#define F_DUPFD  0
#define F_GETFD  1
#define F_SETFD  2
#define F_GETFL  3
#define F_SETFL  4

#define F_SETOWN 8
#define F_GETOWN 9
#define F_SETSIG 10
#define F_GETSIG 11

#if __INTPTR_WIDTH__ == 64

#define F_GETLK64 5
#define F_SETLK64 6
#define F_SETLKW64 7

#else /* __INTPTR_WIDTH__ == 64 */

#define F_GETLK64 12
#define F_SETLK64 13
#define F_SETLKW64 14

#endif

#define F_GETLK F_GETLK64
#define F_SETLK F_SETLK64
#define F_SETLKW F_SETLKW64

#define F_SETOWN_EX 15
#define F_GETOWN_EX 16

#define F_GETOWNER_UIDS 17

#define F_SETLEASE 1024
#define F_GETLEASE 1025
#define F_NOTIFY 1026
#define F_DUPFD_CLOEXEC 1030
#define F_SETPIPE_SZ 1031
#define F_GETPIPE_SZ 1032
#define F_ADD_SEALS 1033
#define F_GET_SEALS 1034

#define F_SEAL_SEAL 0x0001
#define F_SEAL_SHRINK 0x0002
#define F_SEAL_GROW 0x0004
#define F_SEAL_WRITE 0x0008

#define F_OFD_GETLK 36
#define F_OFD_SETLK 37
#define F_OFD_SETLKW 38

#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2

#define FD_CLOEXEC 1

#define AT_FDCWD -100
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR 0x200
#define AT_SYMLINK_FOLLOW 0x400
#define AT_EACCESS 0x200

#if defined(_GNU_SOURCE)
#define AT_NO_AUTOMOUNT 0x800
#define AT_EMPTY_PATH 0x1000
#endif

#if __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE)

#define DN_ACCESS 1
#define DN_MODIFY 2
#define DN_CREATE 4
#define DN_DELETE 8
#define DN_RENAME 16
#define DN_ATTRIB 32
#define DN_MULTISHOT 0x80000000

#define AT_STATX_SYNC_AS_STAT 0x0000
#define AT_STATX_FORCE_SYNC 0x2000
#define AT_STATX_DONT_SYNC 0x4000
#define AT_STATX_SYNC_TYPE 0x6000

#endif /* __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE) */

#if defined(_GNU_SOURCE) || __MLIBC_POSIX2024
struct f_owner_ex {
	int type;
	pid_t pid;
};
#endif /* defined(_GNU_SOURCE) || __MLIBC_POSIX2024 */

#define F_OWNER_TID 0
#define F_OWNER_PID 1
#define F_OWNER_PGRP 2

#define POSIX_FADV_NORMAL 0
#define POSIX_FADV_RANDOM 1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_NOREUSE 5

#define S_IRWXU 0700
#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRWXG 070
#define S_IRGRP 040
#define S_IWGRP 020
#define S_IXGRP 010
#define S_IRWXO 07
#define S_IROTH 04
#define S_IWOTH 02
#define S_IXOTH 01
#define S_ISUID 04000
#define S_ISGID 02000
#define S_ISVTX 01000

#endif /* _ABIBITS_FCNTL_H */
