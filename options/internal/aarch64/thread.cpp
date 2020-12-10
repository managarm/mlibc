#include <mlibc/thread.hpp>

#include <stdint.h>

namespace mlibc {

Tcb *get_current_tcb() {
	uintptr_t ptr;
	asm ("mrs %0, tpidr_el0" : "=r"(ptr));
	return reinterpret_cast<Tcb *>(ptr);
}

} // namespace mlibc
