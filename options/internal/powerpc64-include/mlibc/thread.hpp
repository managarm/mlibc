#pragma once

#include <bits/ensure.h>
#include <mlibc/tcb.hpp>
#include <stdint.h>

namespace mlibc {

inline Tcb *get_current_tcb() {
	// inverse of sys_tcb_set from linux sysdeps.cpp
	uintptr_t thread_ptr;
	asm volatile("mr %0, 13" : "=r"(thread_ptr));
	return reinterpret_cast<Tcb *>(thread_ptr - 0x7000 - sizeof(Tcb));
}

inline uintptr_t get_sp() {
	uintptr_t sp;
	asm volatile("mr %0, 1" : "=r"(sp));
	return sp;
}

} // namespace mlibc
