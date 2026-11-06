#pragma once

#include <stdint.h>

namespace mlibc {

inline void *get_tp() {
	void *ptr;
	// Tcb->selfPointer
	asm volatile ("movq %%fs:0, %0" : "=r"(ptr));
	return ptr;
}

inline uintptr_t get_sp() {
	uintptr_t rsp;
	asm volatile ("mov %%rsp, %0" : "=r"(rsp));
	return rsp;
}

} // namespace mlibc
