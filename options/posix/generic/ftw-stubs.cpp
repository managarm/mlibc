
#include <ftw.h>

#include <bits/ensure.h>

int ftw(const char *path, int (*fn)(const char *, const struct stat *, int), int fd_limit) {
	__ensure(!"ftw() not implemented");
	__builtin_unreachable();
}

int nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *),
		int fd_limit, int flags) {
	__ensure(!"nftw() not implemented");
	__builtin_unreachable();
}

