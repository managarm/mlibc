#pragma once

#include <stdint.h>
#include <mlibc/tcb.hpp>
#include <bits/ensure.h>

namespace mlibc {

inline Tcb *get_current_tcb() {
	uintptr_t tp = (uintptr_t)__builtin_thread_pointer();
	auto tcb = reinterpret_cast<Tcb *>(tp - 0x7000 - sizeof(Tcb));
	__ensure(tcb == tcb->selfPointer);
	return tcb;
}

inline uintptr_t get_sp() {
	uintptr_t sp;
	asm volatile ("move %0, $sp" : "=r"(sp));
	return sp;
}

} // namespace mlibc
