#pragma once

#include <stdint.h>

namespace mlibc {

inline void *get_tp() {
	void *ptr;
	// Tcb->selfPointer
	asm volatile ("movl %%gs:0, %0" : "=r"(ptr));
	return ptr;
}

inline uintptr_t get_sp() {
	uintptr_t esp;
	asm volatile ("mov %%esp, %0" : "=r"(esp));
	return esp;
}

} // namespace mlibc

