
#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <bits/posix/stat.h>
#include <mlibc-config.h>

#if __MLIBC_LINUX_OPTION
#include <bits/linux/linux_stat.h>
#endif /* !__MLIBC_LINUX_OPTION */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int chmod(const char *__pathname, mode_t __mode);
int fchmod(int __fd, mode_t __mode);
int fchmodat(int __fd, const char *__pathname, mode_t __mode, int __flags);
int fstat(int __fd, struct stat *__result);
int fstat64(int __fd, struct stat64 *__result);
int fstatat(int __fd, const char *__restrict __pathname, struct stat *__restrict __buf, int __flags);
int futimens(int __fd, const struct timespec __times[2]);
int lstat(const char *__restrict __pathname, struct stat *__restrict __buf);
int lstat64(const char *__restrict __pathname, struct stat64 *__restrict __buf);
int mkdir(const char *__pathname, mode_t __mode);
int mkdirat(int __dirfd, const char *__pathname, mode_t __mode);
int mkfifo(const char *__pathname, mode_t __mode);
int mkfifoat(int __dirfd, const char *__pathname, mode_t __mode);
int mknod(const char *__pathname, mode_t __mode, dev_t __dev);
int mknodat(int __dirfd, const char *__pathname, mode_t __mode, dev_t __dev);
int stat(const char *__restrict __pathname, struct stat *__restrict __buf);
mode_t umask(mode_t __mode);
int utimensat(int __dirfd, const char *__pathname, const struct timespec __times[2], int __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_STAT_H */

