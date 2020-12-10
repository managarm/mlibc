
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <mlibc/internal-sysdeps.hpp>
#include <mlibc/debug.hpp>

typedef void (*InitPtr)();

extern InitPtr __CTOR_LIST__ [[ gnu::visibility("hidden") ]];
extern InitPtr __CTOR_END__ [[ gnu::visibility("hidden") ]];

extern "C" [[ gnu::visibility("hidden") ]] void __mlibc_do_ctors() {
	auto it = &__CTOR_LIST__;
	while(it != &__CTOR_END__)
		(*it++)();
}

extern "C" [[ gnu::visibility("hidden") ]] void __mlibc_do_dtors() {
	mlibc::sys_libc_log("__mlibc_do_dtors() called");
}

