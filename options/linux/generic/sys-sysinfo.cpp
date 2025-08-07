#include <errno.h>
#include <sys/sysinfo.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>
#include <unistd.h>

int sysinfo(struct sysinfo *info) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sysinfo, -1);
	if(int e = mlibc::sys_sysinfo(info); e) {
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
