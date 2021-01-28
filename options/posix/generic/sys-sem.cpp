
#include <bits/ensure.h>
#include <sys/sem.h>

int semget(key_t, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
