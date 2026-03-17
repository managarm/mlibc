#include <errno.h>
#include <sys/statvfs.h>

#include <bits/ensure.h>
#include <mlibc-config.h>
#include <mlibc/all-sysdeps.hpp>

int statvfs(const char *path, struct statvfs *out) {
	if(int e = mlibc::sysdep_or_enosys<Statvfs>(path, out); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("statvfs")]] int statvfs64(const char *path, struct statvfs64 *out);
#endif /* !__MLIBC_LINUX_OPTION */

int fstatvfs(int fd, struct statvfs *out) {
	if(int e = mlibc::sysdep_or_enosys<Fstatvfs>(fd, out); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("fstatvfs")]] int fstatvfs64(int, struct statvfs64 *);
#endif /* !__MLIBC_LINUX_OPTION */
