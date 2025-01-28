#ifndef _MLIBC_LINUX_SYS_XATTR_H
#define _MLIBC_LINUX_SYS_XATTR_H

#include <sys/types.h>
#include <abi-bits/xattr.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int setxattr(const char *__path, const char *__name, const void *__val, size_t __size,
	int __flags);
int lsetxattr(const char *__path, const char *__name, const void *__val, size_t __size,
	int __flags);
int fsetxattr(int __fd, const char *__name, const void *__val, size_t __size,
	int __flags);

ssize_t getxattr(const char *__path, const char *__name, void *__val, size_t __size);
ssize_t lgetxattr(const char *__path, const char *__name, void *__val, size_t __size);
ssize_t fgetxattr(int __fd, const char *__name, void *__val, size_t __size);

ssize_t listxattr(const char *__path, char *__list, size_t __size);
ssize_t llistxattr(const char *__path, char *__list, size_t __size);
ssize_t flistxattr(int __fd, char *__list, size_t __size);

int removexattr(const char *__path, const char *__name);
int lremovexattr(const char *__path, const char *__name);
int fremovexattr(int __fd, const char *__name);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_LINUX_SYS_XATTR_H */
