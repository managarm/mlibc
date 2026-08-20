#include <astral/syscall.h>
#include <astral/archctl.h>
#include <astral/sysctl.h>
#include <errno.h>

#ifndef MLIBC_BUILDING_RTLD

int sysctl(const int *name, size_t name_len, void *oldp, size_t *old_lenp, void *newp, size_t new_len) {
	long ret;
	long error = syscall(SYSCALL_SYSCTL, &ret, (uint64_t)name, name_len, (uint64_t)oldp, (uint64_t)old_lenp, (uint64_t)newp, new_len);
	if (error) {
		errno = error;
		return -1;
	}

	return 0;
}

int arch_ctl(int func, void *arg) {
	long ret;
	long error = syscall(SYSCALL_ARCHCTL, &ret, func, (uint64_t)arg);
	if (error)
		errno = error;
	return error ? -1 : ret;
}

#endif
