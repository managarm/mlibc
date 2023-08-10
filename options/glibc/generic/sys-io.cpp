#include <errno.h>
#include <sys/io.h>

#include <bits/ensure.h>
#include <mlibc/glibc-sysdeps.hpp>

int ioperm(unsigned long int from, unsigned long int num, int turn_on) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_ioperm, -1);

	if(int e = sysdep(from, num, turn_on); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int iopl(int level) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_iopl, -1);

	if(int e = sysdep(level); e) {
		errno = e;
		return -1;
	}
	return 0;
}
