#include <errno.h>
#include <sys/io.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

int ioperm(unsigned long int from, unsigned long int num, int turn_on) {
	if(int e = mlibc::sysdep_or_enosys<Ioperm>(from, num, turn_on); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int iopl(int level) {
	if(int e = mlibc::sysdep_or_enosys<Iopl>(level); e) {
		errno = e;
		return -1;
	}
	return 0;
}
