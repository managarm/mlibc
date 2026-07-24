// Taken from mlibc linux option
#include <errno.h>
#include <stdarg.h>
#include <sys/ptrace.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <stdarg.h>
#include <sys/syscall.h>
#include <sys/types.h>

namespace {
int __ethereal_ptrace(long req, pid_t pid, void *addr, void *data, long *out) {
	long err = SYSCALL4(SYS_PTRACE, req, pid, addr, data);
	if (err < 0)
		return -err;
	*out = err;
	return 0;
}
} // namespace

long ptrace(int req, ...) {
	va_list ap;

	va_start(ap, req);
	auto pid = va_arg(ap, pid_t);
	auto addr = va_arg(ap, void *);
	auto data = va_arg(ap, void *);
	va_end(ap);

	long ret;
	if (req > 0 && req < 4) {
		data = &ret;
	}

	long out;
	if (int e = __ethereal_ptrace(req, pid, addr, data, &out); e) {
		errno = e;
		return -1;
	} else if (req > 0 && req < 4) {
		errno = 0;
		return ret;
	}

	return out;
}
