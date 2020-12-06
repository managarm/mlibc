#include <mlibc/debug.hpp>
#include <bits/ensure.h>

extern "C" void __mlibc_log_setjmp_error() {
	__ensure(!"setjmp, sigsetjmp, longjmp are not implemented");
}
