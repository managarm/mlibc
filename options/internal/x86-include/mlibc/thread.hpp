#pragma once

#include <stdint.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>

namespace mlibc {

inline Tcb *get_current_tcb() {
	if constexpr(IsImplemented<TcbGet>)
		return static_cast<Tcb *>(sysdep<TcbGet>());

	uintptr_t ptr;
	asm volatile ("movl %%gs:0, %0" : "=r"(ptr));
	return reinterpret_cast<Tcb *>(ptr);
}

inline uintptr_t get_sp() {
	uintptr_t esp;
	asm volatile ("mov %%esp, %0" : "=r"(esp));
	return esp;
}

} // namespace mlibc
