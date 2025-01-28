#pragma once

#include <stdint.h>
#include <mlibc/internal-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <bits/ensure.h>

namespace mlibc {

extern "C" void *__m68k_read_tp();

inline Tcb *get_current_tcb() {
	// On m68k, the end of the TCB is 0x7000 below the thread pointer.
	void *ptr = __m68k_read_tp();
	return reinterpret_cast<Tcb *>((uintptr_t)ptr - 0x7000 - sizeof(Tcb));
}

inline uintptr_t get_sp() {
	uintptr_t sp;
	asm volatile ("move.l %%sp, %0" : "=r"(sp));
	return sp;
}

} // namespace mlibc
