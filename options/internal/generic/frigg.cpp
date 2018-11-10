
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

extern "C" void frg_panic(const char *mstr) {
	mlibc::sys_libc_log(mstr);
	mlibc::sys_libc_panic();
}

