
#include <stdarg.h>
#include <errno.h>
#include <bits/ensure.h>
#include <sys/prctl.h>

#include <mlibc/debug.hpp>

#include "mlibc/linux-sysdeps.hpp"

int prctl(int op, ...) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_prctl, -1);

	int val;
	va_list ap;
	va_start(ap, op);
	if(int e = mlibc::sys_prctl(op, ap, &val); e) {
		errno = e;
		return -1;
	}
	va_end(ap);

	return val;
}

