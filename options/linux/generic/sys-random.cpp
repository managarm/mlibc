
#include <bits/ensure.h>
#include <errno.h>
#include <sys/random.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

ssize_t getrandom(void *buffer, size_t max_size, unsigned int flags) {
	if(flags & ~(GRND_RANDOM | GRND_NONBLOCK)) {
		errno = EINVAL;
		return -1;
	}
	if(int e = mlibc::sysdep_or_enosys<GetEntropy>(buffer, max_size); e) {
		errno = e;
		return -1;
	}
	return max_size;
}
