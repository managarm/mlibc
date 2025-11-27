#include <bits/linux/linux_unistd.h>
#include <bits/ensure.h>

#include <errno.h>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/linux-sysdeps.hpp>
#include <unistd.h>

int vhangup(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int syncfs(int fd) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_syncfs, -1);
	if(int e = mlibc::sys_syncfs(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}
