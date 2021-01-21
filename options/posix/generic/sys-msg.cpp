
#include <bits/ensure.h>
#include <sys/msg.h>

int msgget(key_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
