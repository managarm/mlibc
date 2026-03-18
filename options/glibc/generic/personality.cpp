#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/personality.h>

int personality(unsigned long persona) {
	int out = 0;
	if(int e = mlibc::sysdep_or_enosys<Personality>(persona, &out); e) {
		errno = e;
		return -1;
	}
	return out;
}
