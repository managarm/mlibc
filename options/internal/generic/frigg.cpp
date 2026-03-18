
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>

extern "C" void frg_panic(const char *mstr) {
//	mlibc::sysdep<LibcLog>("mlibc: Call to frg_panic");
	mlibc::sysdep<LibcLog>(mstr);
	mlibc::sysdep<LibcPanic>();
}

extern "C" void frg_log(const char *mstr) {
	mlibc::sysdep<LibcLog>(mstr);
}
