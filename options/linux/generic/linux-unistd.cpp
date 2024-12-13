#include <bits/linux/linux_unistd.h>
#include <bits/ensure.h>

#include <errno.h>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/linux-sysdeps.hpp>
#include <unistd.h>

int dup3(int oldfd, int newfd, int flags) {
	if(oldfd == newfd) {
		errno = EINVAL;
		return -1;
	}
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_dup2, -1);
	if(int e = mlibc::sys_dup2(oldfd, flags, newfd); e) {
		errno = e;
		return -1;
	}
	return newfd;
}

int vhangup(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getdtablesize(void){
	return sysconf(_SC_OPEN_MAX);
}

int syncfs(int fd) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_syncfs, -1);
	if(int e = mlibc::sys_syncfs(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}
