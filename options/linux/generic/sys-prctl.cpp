
#include <stdarg.h>
#include <errno.h>
#include <bits/ensure.h>
#include <sys/prctl.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int prctl(int op, ...) {
	int val;
	va_list ap;
	va_start(ap, op);
	if(int e = mlibc::sysdep_or_enosys<Prctl>(op, ap, &val); e) {
		va_end(ap);
		errno = e;
		return -1;
	}
	va_end(ap);

	return val;
}

