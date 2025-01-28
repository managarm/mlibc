#pragma once

#include <stdint.h>
#include <mlibc/tcb.hpp>

namespace mlibc {

inline Tcb *get_current_tcb() {
	uintptr_t ptr;
	asm volatile ("movq %%fs:0, %0" : "=r"(ptr));
	return reinterpret_cast<Tcb *>(ptr);
}

inline uintptr_t get_sp() {
	uintptr_t rsp;
	asm volatile ("mov %%rsp, %0" : "=r"(rsp));
	return rsp;
}

} // namespace mlibc
