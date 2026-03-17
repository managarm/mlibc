#include <errno.h>
#include <sys/sysinfo.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <unistd.h>

int sysinfo(struct sysinfo *info) {
	if(int e = mlibc::sysdep_or_enosys<Sysinfo>(info); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int get_nprocs(void) {
	return sysconf(_SC_NPROCESSORS_ONLN);
}

int get_nprocs_conf(void) {
	return sysconf(_SC_NPROCESSORS_CONF);
}
