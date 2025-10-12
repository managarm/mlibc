#ifndef _ABIBITS_FCNTL_H
#define _ABIBITS_FCNTL_H

#include <abi-bits/pid_t.h>
#include <mlibc-config.h>

#define O_RDONLY (1 << 0)
#define O_WRONLY (1 << 1)
#define O_CREAT (1 << 6)
#define O_EXCL (1 << 7)
#define O_NOCTTY (1 << 8)
#define O_TRUNC (1 << 9)
#define O_APPEND (1 << 10)
#define O_NONBLOCK (1 << 11)
#define O_DSYNC (1 << 12)
#define O_ASYNC (1 << 13)
#define O_DIRECT (1 << 14)
#define O_LARGEFILE (1 << 15)
#define O_DIRECTORY (1 << 16)
#define O_NOFOLLOW (1 << 17)
#define O_NOATIME (1 << 18)
#define O_CLOEXEC (1 << 19)
#define O_PATH (1 << 21)
#define O_TMPFILE (1 << 22)
#define O_SYNC (O_DIRECTORY | O_TMPFILE)
#define O_RSYNC O_SYNC

#define O_EXEC O_PATH
#define O_SEARCH O_PATH

#define O_RDWR (O_RDONLY | O_WRONLY)
#define O_ACCMODE (O_RDWR | O_PATH)

#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4

#define F_SETOWN 8
#define F_GETOWN 9
#define F_SETSIG 10
#define F_GETSIG 11

#define F_GETLK 5
#define F_SETLK 6
#define F_SETLK64 F_SETLK
#define F_SETLKW 7
#define F_SETLKW64 F_SETLKW

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

#define F_SEAL_SEAL (1 << 0)
#define F_SEAL_SHRINK (1 << 1)
#define F_SEAL_GROW (1 << 2)
#define F_SEAL_WRITE (1 << 3)

#define F_OFD_GETLK 36
#define F_OFD_SETLK 37
#define F_OFD_SETLKW 38

#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2

#define FD_CLOEXEC 1

#define AT_FDCWD (-100)

#define AT_SYMLINK_NOFOLLOW (1 << 8)
#define AT_SYMLINK_FOLLOW (1 << 9)
#define AT_REMOVEDIR (1 << 10)
#define AT_EACCESS (1 << 11)
#define AT_NO_AUTOMOUNT (1 << 12)
#define AT_EMPTY_PATH (1 << 13)

#define DN_ACCESS 1
#define DN_MODIFY 2
#define DN_CREATE 4
#define DN_DELETE 8
#define DN_RENAME 16
#define DN_ATTRIB 32
#define DN_MULTISHOT 0x80000000

#define AT_STATX_SYNC_AS_STAT 0x0000
#define AT_STATX_FORCE_SYNC (1 << 14)
#define AT_STATX_DONT_SYNC (1 << 15)
#define AT_STATX_SYNC_TYPE (AT_STATX_FORCE_SYNC | AT_STATX_DONT_SYNC)

#if defined(_GNU_SOURCE)
struct f_owner_ex {
	int type;
	pid_t pid;
};
#endif /* _GNU_SOURCE */

#define F_OWNER_TID 0

#define POSIX_FADV_NORMAL 0
#define POSIX_FADV_RANDOM 1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_NOREUSE 5

#endif
