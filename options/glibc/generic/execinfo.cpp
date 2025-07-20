#include <execinfo.h>
#include <unwind.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

namespace {

struct UnwindState {
	void **frames;
	int count;
	int current_frame = 0;
};

_Unwind_Reason_Code trace(_Unwind_Context *context, void *arg) {
	uintptr_t ip = _Unwind_GetIP(context);

	if (ip) {
#if defined(__x86_64__) || defined(__i386__)
		ip--;
#elif defined(__aarch64__) || defined(__loongarch64)
		ip -= 4;
#elif defined(__riscv) || defined(__m68k__)
		ip -= 2;
#else
#warning "Missing support for architecture"
		ip--;
#endif
	}

	UnwindState *state = static_cast<UnwindState *>(arg);
	state->frames[state->current_frame++] = reinterpret_cast<void *>(ip);
	return (state->current_frame >= state->count) ? _URC_END_OF_STACK : _URC_NO_REASON;
}

} // namespace

int backtrace(void **buffer, int size) {
	if (size <= 0)
		return 0;

	UnwindState state{buffer, size};
	_Unwind_Backtrace(trace, &state);
	return state.current_frame;
}

char **backtrace_symbols(void *const *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void backtrace_symbols_fd(void *const *, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
