
#include <sys/ptrace.h>
#include <stdarg.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

long ptrace(int req, ...) {
	va_list ap;

	va_start(ap, req);
	auto pid = va_arg(ap, pid_t);
	auto addr = va_arg(ap, void *);
	auto data = va_arg(ap, void *);
	va_end(ap);

	long ret;
	if(req > 0 && req < 4) {
		data = &ret;
	}

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_ptrace, -1);
	long out;
	if(int e = mlibc::sys_ptrace(req, pid, addr, data, &out); e) {
		errno = e;
		return -1;
	} else if(req > 0 && req < 4) {
		errno = 0;
		return ret;
	}

	return out;
}

