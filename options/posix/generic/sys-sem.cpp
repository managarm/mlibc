
#include <bits/ensure.h>
#include <sys/sem.h>

int semget(key_t, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int semop(int, struct sembuf *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int semctl(int, int, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
