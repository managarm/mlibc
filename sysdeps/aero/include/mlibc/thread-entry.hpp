#pragma once

#include <mlibc/tcb.hpp>

extern "C" void __mlibc_start_thread(void);
extern "C" void __mlibc_enter_thread(void *entry, void *user_arg, Tcb *tcb);

namespace mlibc {
int prepare_stack(void **stack, void *entry, void *user_arg, void *tcb, size_t *stack_size,
		size_t *guard_size);
} // namespace mlibc
