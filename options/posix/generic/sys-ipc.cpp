#include <sys/ipc.h>

#include <bits/ensure.h>

key_t ftok(const char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
