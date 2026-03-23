
#include <errno.h>
#include <sys/statfs.h>
#include <bits/ensure.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int statfs(const char *path, struct statfs *buf) {
	if(int e = mlibc::sysdep_or_enosys<Statfs>(path, buf); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fstatfs(int fd, struct statfs *buf) {
	if (int e = mlibc::sysdep_or_enosys<Fstatfs>(fd, buf); e) {
		errno = e;
		return -1;
	}
	return 0;
}

[[gnu::alias("statfs")]] int statfs64(const char *, struct statfs64 *);
[[gnu::alias("fstatfs")]] int fstatfs64(int, struct statfs64 *);

