#pragma once

#include <stdint.h>

namespace mlibc {

inline void *get_tp() {
	return __builtin_thread_pointer();
}

inline uintptr_t get_sp() {
	uintptr_t sp;
	asm volatile ("mv %0, sp" : "=r"(sp));
	return sp;
}

} // namespace mlibc
