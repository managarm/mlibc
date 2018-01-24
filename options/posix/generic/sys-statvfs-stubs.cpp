
#include <sys/statvfs.h>
#include <bits/ensure.h>

int statvfs(const char *__restrict, struct statvfs *__restirct) {
	__ensure(!"dlclose() not implemented");
	__builtin_unreachable();
}

int fstatvfs(int, struct statvfs *) {
	__ensure(!"dlclose() not implemented");
	__builtin_unreachable();
}

