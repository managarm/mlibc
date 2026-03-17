
#include <bits/ensure.h>
#include <errno.h>
#include <mlibc-config.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/file.h>

int flock(int fd, int opt) {
	if(int e = mlibc::sysdep_or_enosys<Flock>(fd, opt); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("flock")]] int flock64(int fd, int opt);
#endif /* !__MLIBC_LINUX_OPTION */
