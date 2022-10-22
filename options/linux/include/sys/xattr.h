#ifndef _MLIBC_LINUX_SYS_XATTR_H
#define _MLIBC_LINUX_SYS_XATTR_H

#include <sys/types.h>
#include <abi-bits/xattr.h>

#ifdef __cplusplus
extern "C" {
#endif

int setxattr(const char *path, const char *name, const void *val, size_t size,
	int flags);
int lsetxattr(const char *path, const char *name, const void *val, size_t size,
	int flags);
int fsetxattr(int fd, const char *name, const void *val, size_t size,
	int flags);

ssize_t getxattr(const char *path, const char *name, void *val, size_t size);
ssize_t lgetxattr(const char *path, const char *name, void *val, size_t size);
ssize_t fgetxattr(int fd, const char *name, void *val, size_t size);

ssize_t listxattr(const char *path, char *list, size_t size);
ssize_t llistxattr(const char *path, char *list, size_t size);
ssize_t flistxattr(int fd, char *list, size_t size);

int removexattr(const char *path, const char *name);
int lremovexattr(const char *path, const char *name);
int fremovexattr(int fd, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_LINUX_SYS_XATTR_H */
