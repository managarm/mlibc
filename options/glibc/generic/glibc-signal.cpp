#include <errno.h>
#include <signal.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/glibc-sysdeps.hpp>

int tgkill(int tgid, int tid, int sig) {
	if(!mlibc::sys_tgkill) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_tgkill(tgid, tid, sig); e) {
		errno = e;
		return -1;
	}
	return 0;
}
