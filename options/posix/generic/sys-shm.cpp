#include <sys/shm.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

void *shmat(int shmid, const void *shmaddr, int shmflg) {
	void *ret;
	if(int e = mlibc::sysdep_or_enosys<Shmat>(&ret, shmid, shmaddr, shmflg); e) {
		errno = e;
		return ((void *)-1);
	}
	return ret;
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf) {
	int ret;
	if(int e = mlibc::sysdep_or_enosys<Shmctl>(&ret, shmid, cmd, buf); e) {
		errno = e;
		return -1;
	}
	return ret;
}

int shmdt(const void *shmaddr) {
	if(int e = mlibc::sysdep_or_enosys<Shmdt>(shmaddr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int shmget(key_t key, size_t size, int shmflg) {
	int ret;
	if(int e = mlibc::sysdep_or_enosys<Shmget>(&ret, key, size, shmflg); e) {
		errno = e;
		return -1;
	}
	return ret;
}
