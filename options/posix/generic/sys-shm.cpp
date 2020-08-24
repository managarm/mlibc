#include <sys/shm.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

void *shmat(int, const void *, int) {
	__ensure(!"Function is not implemented");
	__builtin_unreachable();
}

int shmctl(int, int, struct shmid_ds *) {
	__ensure(!"Function is not implemented");
	__builtin_unreachable();
}

int shmdt(const void *) {
	__ensure(!"Function is not implemented");
	__builtin_unreachable();
}

int shmget(key_t, size_t, int) {
	mlibc::infoLogger() << "mlibc: shmget() is a no-op!" << frg::endlog;
	return -1;
}
