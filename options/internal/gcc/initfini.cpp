
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <mlibc/internal-sysdeps.hpp>
#include <mlibc/debug.hpp>

typedef void (*InitPtr)();

extern InitPtr __CTOR_LIST__ [[ gnu::visibility("hidden") ]] [];
extern InitPtr __CTOR_END__ [[ gnu::visibility("hidden") ]] [];

extern "C" [[ gnu::visibility("hidden") ]] void __mlibc_do_ctors() {
	const size_t n = __CTOR_END__ - __CTOR_LIST__;
	if(!n)
		return;

	size_t num = frg::min(n - 1, size_t(__CTOR_LIST__[0]));

	for(size_t i = num; i >= 1; i--) {
		__CTOR_LIST__[i]();
	}
}

extern "C" [[ gnu::visibility("hidden") ]] void __mlibc_do_dtors() {
	mlibc::sys_libc_log("__mlibc_do_dtors() called");
}

