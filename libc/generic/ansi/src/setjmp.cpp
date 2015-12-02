
#include <setjmp.h>

#include <mlibc/ensure.h>

int setjmp(jmp_buf buffer) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void longjmp(jmp_buf buffer, int value) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

