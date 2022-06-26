#include <errno.h>
#include <signal.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/glibc-sysdeps.hpp>

int tgkill(int tgid, int tid, int sig) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_tgkill, -1);
	if(int e = mlibc::sys_tgkill(tgid, tid, sig); e) {
		errno = e;
		return -1;
	}
	return 0;
}
