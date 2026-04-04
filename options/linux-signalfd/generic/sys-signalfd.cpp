
#include <errno.h>
#include <sys/signalfd.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

static_assert(sizeof(signalfd_siginfo) == 128, "signalfd_siginfo must be exactly 128 bytes!");

int signalfd(int fd, const sigset_t *mask, int flags) {
	if(int e = mlibc::sysdep_or_enosys<SignalfdCreate>(mask, flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

