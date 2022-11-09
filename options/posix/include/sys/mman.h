#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <mlibc-config.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/vm-flags.h>
#include <bits/off_t.h>
#include <bits/size_t.h>

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

int shm_open(const char *, int, mode_t);
int shm_unlink(const char *);

// Linux extension:
void *mremap(void *, size_t, size_t, int, ...);
int remap_file_pages(void *, size_t, int, size_t, int);

#if __MLIBC_LINUX_OPTION
int memfd_create(const char *, unsigned int);
int madvise(void *, size_t, int);
int mincore(void *, size_t, unsigned char *);
#endif /* __MLIBC_LINUX_OPTION */

#ifdef __cplusplus
}
#endif

#endif // _SYS_MMAN_H
