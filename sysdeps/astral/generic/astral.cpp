#include <astral/syscall.h>
#include <astral/archctl.h>
#include <errno.h>

#ifndef MLIBC_BUILDING_RTLD

int syscall_archctl(int func, void *arg) {
	long ret;
	long error = syscall(SYSCALL_ARCHCTL, &ret, func, (uint64_t)arg);
	if(error)
		errno = error;
	return error ? -1 : ret;
}

#endif
