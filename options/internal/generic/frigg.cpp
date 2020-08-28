
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/internal-sysdeps.hpp>

extern "C" void frg_panic(const char *mstr) {
//	mlibc::sys_libc_log("mlibc: Call to frg_panic");
	mlibc::sys_libc_log(mstr);
	mlibc::sys_libc_panic();
}

extern "C" void frg_log(const char *mstr) {
	mlibc::sys_libc_log(mstr);
}
