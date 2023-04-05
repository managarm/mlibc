#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/glibc-sysdeps.hpp>
#include <sys/personality.h>

int personality(unsigned long persona) {
	int out = 0;
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_personality, -1);

	if(int e = sysdep(persona, &out); e) {
		errno = e;
		return -1;
	}
	return out;
}
