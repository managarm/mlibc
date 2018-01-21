
#include <sys/statfs.h>
#include <mlibc/ensure.h>

int statfs(const char *, struct statfs *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fstatfs(int, struct statfs *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

