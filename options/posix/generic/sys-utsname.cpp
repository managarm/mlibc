
#include <string.h>
#include <sys/utsname.h>
#include <errno.h>

#include <bits/ensure.h>
#include <internal-config.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int uname(struct utsname *p) {
	if (p == nullptr) {
		errno = EFAULT;
		return -1;
	}

	if(int e = mlibc::sysdep_or_enosys<Uname>(p); e) {
		errno = e;
		return -1;
	}
	return 0;
}

