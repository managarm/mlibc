#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <bits/posix/off_t.h>
#include <bits/size_t.h>

#define PROT_NONE 0x00
#define PROT_READ 0x01
#define PROT_WRITE 0x02
#define PROT_EXEC 0x04

#define MAP_PRIVATE 0x01
#define MAP_SHARED 0x02
#define MAP_FIXED 0x04

#define MAP_FAILED ((void *)(-1))

// posix extension:
#define MAP_ANON 0x08
#define MAP_ANONYMOUS 0x08

#define MS_ASYNC 0x01
#define MS_SYNC 0x02
#define MS_INVALIDATE 0x04

#define MCL_CURRENT 0x01
#define MCL_FUTURE 0x02

#define POSIX_MADV_NORMAL 1
#define POSIX_MADV_SEQUENTIAL 2
#define POSIX_MADV_RANDOM 3
#define POSIX_MADV_DONTNEED 4
#define POSIX_MADV_WILLNEED 5

// Missing: posix_typed_mem_open(), POSIX_TYPED constants and related stuff.

#ifdef __cplusplus
extern "C" {
#endif

void *mmap(void *, size_t, int, int, int, off_t);
int mprotect(void *, size_t, int);
int munmap(void *, size_t);

int mlock(const void *, size_t);
int mlockall(int);
int munlock(const void *, size_t);
int munlockall(void);

int posix_madvise(void *, size_t, int);
int msync(void *, size_t, int);

#ifdef __cplusplus
}
#endif

#endif // _SYS_MMAN_H
