
#include <ftw.h>

#include <bits/ensure.h>

int ftw(const char *, int (*fn)(const char *, const struct stat *, int), int) {
	(void)fn;
	__ensure(!"ftw() not implemented");
	__builtin_unreachable();
}

int nftw(const char *, int (*fn)(const char *, const struct stat *, int, struct FTW *),
		int, int) {
	(void)fn;
	__ensure(!"nftw() not implemented");
	__builtin_unreachable();
}

