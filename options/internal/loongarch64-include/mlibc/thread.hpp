#pragma once

#include <stdint.h>
#include <mlibc/tcb.hpp>
#include <bits/ensure.h>

namespace mlibc {

inline Tcb *get_current_tcb() {
	// On LoongArch, the TCB is below the thread pointer.
	uintptr_t tp = (uintptr_t)__builtin_thread_pointer();
	auto tcb = reinterpret_cast<Tcb *>(tp - sizeof(Tcb));
	__ensure(tcb == tcb->selfPointer);
	return tcb;
}

inline uintptr_t get_sp() {
	uintptr_t sp;
	asm volatile ("move %0, $sp" : "=r"(sp));
	return sp;
}

} // namespace mlibc
