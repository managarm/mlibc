#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <abi-bits/vm-flags.h>
#include <bits/off_t.h>
#include <bits/size_t.h>

#define MAP_FAILED ((void *)(-1))

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

// Linux extension:
#define MREMAP_MAYMOVE 1
#define MREMAP_FIXED 2

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

// Linux extension:
void *mremap(void *, size_t, size_t, int, ...);
int remap_file_pages(void *, size_t, int, size_t, int);

#ifdef __cplusplus
}
#endif

#endif // _SYS_MMAN_H
