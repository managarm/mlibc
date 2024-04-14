#include <ucontext.h>
#include <bits/ensure.h>

int getcontext(ucontext_t *) {
	__ensure(!"Not implemented!");
	__builtin_unreachable();
}
int setcontext(const ucontext_t *) {
	__ensure(!"Not implemented!");
	__builtin_unreachable();
}
void makecontext(ucontext_t *, void (*)(), int, ...) {
	__ensure(!"Not implemented!");
	__builtin_unreachable();
}
int swapcontext(ucontext_t *, const ucontext_t *) {
	__ensure(!"Not implemented!");
	__builtin_unreachable();
}
