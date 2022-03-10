#include <bits/ensure.h>
#include <sys/fsuid.h>

int setfsuid(uid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setfsgid(gid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
