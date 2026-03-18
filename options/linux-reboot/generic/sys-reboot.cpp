#include <errno.h>
#include <sys/reboot.h>
#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

int reboot(int what) {
	if (int e = mlibc::sysdep_or_enosys<Reboot>(what); e) {
		errno = e;
		return -1;
	}
	return 0;
}
