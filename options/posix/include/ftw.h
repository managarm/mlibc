
#ifndef _FTW_H
#define _FTW_H

#include <sys/stat.h>

#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_DP 4
#define FTW_NS 5
#define FTW_SL 6
#define FTW_SLN 7

#define FTW_PHYS 1
#define FTW_MOUNT 2
#define FTW_DEPTH 4
#define FTW_CHDIR 8

#define FTW_CONTINUE 0

#ifdef __cplusplus
extern "C" {
#endif

struct FTW {
	int base;
	int level;
};

#ifndef __MLIBC_ABI_ONLY

/* Removed in POSIX 2024 */
#if defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024)
int ftw(const char *__dirpath, int (*__fn)(const char *__fpath, const struct stat *__sb, int __typeflag),
		int __nopenfd);
#endif

int nftw(const char *__dirpath, int (*__fn)(const char *__fpath, const struct stat *__sb, int __typeflag,
		struct FTW *__ftwbuf), int __nopenfd, int __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _FTW_H */

