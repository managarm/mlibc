#include <sys/shm.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

void *shmat(int shmid, const void *shmaddr, int shmflg) {
	void *ret;
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_shmat, ((void *)-1));
	if(int e = sysdep(&ret, shmid, shmaddr, shmflg); e) {
		errno = e;
		return ((void *)-1);
	}
	return ret;
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf) {
	int ret;
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_shmctl, -1);
	if(int e = sysdep(&ret, shmid, cmd, buf); e) {
		errno = e;
		return -1;
	}
	return ret;
}

int shmdt(const void *shmaddr) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_shmdt, -1);
	if(int e = sysdep(shmaddr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int shmget(key_t key, size_t size, int shmflg) {
	int ret;
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_shmget, -1);
	if(int e = sysdep(&ret, key, size, shmflg); e) {
		errno = e;
		return -1;
	}
	return ret;
}
