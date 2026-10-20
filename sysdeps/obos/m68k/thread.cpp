#include <mlibc/tcb.hpp>
#include <obos/syscall.h>

#define Sys_SetTCB 0x80000000
#define Sys_GetTCB 0x80000001

extern "C"
#if MLIBC_BUILDING_RTLD
    __attribute__((visibility("hidden")))
#endif
    void *__m68k_read_tp() {
	// see mlibc::get_current_tcb

	// TODO (mishakov): I am not sure if the kernel (or whoever) uses this anywhere else, so this
	// stays as is despite the sysdeps' changes...
	return static_cast<void *>(mlibc::tcb_to_tp(reinterpret_cast<Tcb *>(syscall0(Sys_GetTCB))));
}

namespace mlibc {
int Sysdeps<TpSet>::operator()(void *pointer) {
	// TODO: see __m68k_read_tp() above
	syscall1(Sys_SetTCB, mlibc::tp_to_tcb(pointer));
	return 0;
}
} // namespace mlibc
