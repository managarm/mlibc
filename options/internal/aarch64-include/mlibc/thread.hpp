#pragma once

#include <stdint.h>

namespace mlibc {

inline void *get_tp() {
	void *ptr;
	asm volatile ("mrs %0, tpidr_el0" : "=r"(ptr));
	return ptr;
}

inline uintptr_t get_sp() {
	uintptr_t sp;
	asm volatile ("mov %0, sp" : "=r"(sp));
	return sp;
}

} // namespace mlibc
