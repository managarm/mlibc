#include <errno.h>
#include <signal.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>

int tgkill(int tgid, int tid, int sig) {
	if(int e = mlibc::sysdep_or_enosys<Tgkill>(tgid, tid, sig); e) {
		errno = e;
		return -1;
	}
	return 0;
}
