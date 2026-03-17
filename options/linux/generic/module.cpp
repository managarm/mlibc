#include <errno.h>
#include <module.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int init_module(void *module, unsigned long length, const char *args) {
	if(int e = mlibc::sysdep_or_enosys<InitModule>(module, length, args); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int delete_module(const char *name, unsigned flags) {
	if(int e = mlibc::sysdep_or_enosys<DeleteModule>(name, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}
