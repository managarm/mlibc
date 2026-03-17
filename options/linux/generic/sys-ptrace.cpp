
#include <sys/ptrace.h>
#include <stdarg.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

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

	long out;
	if(int e = mlibc::sysdep_or_enosys<Ptrace>(req, pid, addr, data, &out); e) {
		errno = e;
		return -1;
	} else if(req > 0 && req < 4) {
		errno = 0;
		return ret;
	}

	return out;
}

