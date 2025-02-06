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

#ifndef __MLIBC_ABI_ONLY

void *mmap(void *__addr, size_t __size, int __prot, int __flags, int __fd, off_t __offset);
void *mmap64(void *__addr, size_t __size, int __prot, int __flags, int __fd, off64_t __offset);
int mprotect(void *__addr, size_t __size, int __prot);
int munmap(void *__addr, size_t __size);

int mlock(const void *__addr, size_t __size);
int mlockall(int __flags);
int munlock(const void *__addr, size_t __size);
int munlockall(void);

int posix_madvise(void *__addr, size_t __size, int __advise);
int msync(void *__addr, size_t __size, int __flags);

int shm_open(const char *__name, int __oflag, mode_t __mode);
int shm_unlink(const char *__name);

#if __MLIBC_LINUX_OPTION
void *mremap(void *__old_address, size_t __old_size, size_t __new_size, int __flags, ...);
int remap_file_pages(void *__addr, size_t __size, int __prot, size_t __pgoff, int __flags);
int memfd_create(const char *__name, unsigned int __flags);
int madvise(void *__addr, size_t __size, int __advise);
int mincore(void *__addr, size_t __size, unsigned char *__vec);
#endif /* __MLIBC_LINUX_OPTION */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MMAN_H */
