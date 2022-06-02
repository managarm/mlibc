#include <mlibc/thread.hpp>

#include <stdint.h>

namespace mlibc {

Tcb *get_current_tcb() {
	uintptr_t ptr;
	asm ("mv %0, tp" : "=r"(ptr));

	// On RISC-V, the TCB is below the thread pointer.
	// TODO: This may be wrong if the TLS section has stricter alignment requirements.
	return reinterpret_cast<Tcb *>(ptr - sizeof(Tcb));
}

} // namespace mlibc
