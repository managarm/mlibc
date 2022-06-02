#include <mlibc/thread.hpp>

#include <stdint.h>

namespace mlibc {

Tcb *get_current_tcb() {
	uintptr_t ptr;
	asm ("mv %0, tp" : "=r"(ptr));
	return reinterpret_cast<Tcb *>(ptr - sizeof(Tcb));
}

} // namespace mlibc
