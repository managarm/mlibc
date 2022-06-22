#pragma once

#include <stdint.h>
#include <mlibc/tcb.hpp>

namespace mlibc {

inline Tcb *get_current_tcb() {
	uintptr_t ptr = (uintptr_t)__builtin_thread_pointer();

	// On RISC-V, the TCB is below the thread pointer.
	// TODO: This may be wrong if the TLS section has stricter alignment requirements.
	return reinterpret_cast<Tcb *>(ptr - sizeof(Tcb));
}

} // namespace mlibc
