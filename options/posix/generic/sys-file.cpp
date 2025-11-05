
#include <sys/file.h>
#include <mlibc/posix-sysdeps.hpp>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc-config.h>

int flock(int fd, int opt) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_flock, -1);
	if(int e = mlibc::sys_flock(fd, opt); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("flock")]] int flock64(int fd, int opt);
#endif /* !__MLIBC_LINUX_OPTION */
