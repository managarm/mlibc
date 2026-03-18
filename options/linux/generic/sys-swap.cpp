#include <errno.h>
#include <sys/swap.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int swapon(const char *path, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Swapon>(path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int swapoff(const char *path) {
	if(int e = mlibc::sysdep_or_enosys<Swapoff>(path); e) {
		errno = e;
		return -1;
	}
	return 0;
}
