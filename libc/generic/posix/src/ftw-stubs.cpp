
#include <ftw.h>

#include <mlibc/ensure.h>

int nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *),
		int fd_limit, int flags) {
	__ensure(!"nftw() not implemented");
	__builtin_unreachable();
}

