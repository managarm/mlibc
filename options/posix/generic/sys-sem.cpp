
#include <bits/ensure.h>
#include <errno.h>
#include <limits.h>
#include <sys/sem.h>

#include <mlibc/all-sysdeps.hpp>

int semget(key_t key, int n, int fl) {
	if(n > USHRT_MAX) {
		errno = EINVAL;
		return -1;
	}

	int id = 0;
	if(int e = mlibc::sysdep_or_enosys<Semget>(key, n, fl, &id); e) {
		errno = e;
		return -1;
	}
	return id;
}

int semop(int, struct sembuf *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int semctl(int id, int num, int cmd, ...) {
	union semun semun;
	int ret = 0;

	va_list ap;
	va_start(ap, cmd);
	semun = va_arg(ap, union semun);
	va_end(ap);

	if(int e = mlibc::sysdep_or_enosys<Semctl>(id, num, cmd, semun.buf, &ret); e) {
		errno = e;
		return -1;
	}

	return ret;
}
