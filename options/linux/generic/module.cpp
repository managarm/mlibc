#include <errno.h>
#include <module.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int init_module(void *module, unsigned long length, const char *args) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_init_module, -1);
	if(int e = mlibc::sys_init_module(module, length, args); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int delete_module(const char *name, unsigned flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_delete_module, -1);
	if(int e = mlibc::sys_delete_module(name, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}
