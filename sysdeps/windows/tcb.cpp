#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>

namespace {
Tcb initial_tcb;
}

#if MLIBC_STATIC_BUILD
namespace mlibc {
bool tcb_available_flag = false;
}
#endif

extern "C" void __mlibc_windows_init_tcb() {
	initial_tcb.selfPointer = &initial_tcb;
	initial_tcb.tid = mlibc::sysdep<FutexTid>();
	if(mlibc::sysdep<TcbSet>(&initial_tcb))
		__builtin_trap();
#if MLIBC_STATIC_BUILD
	mlibc::tcb_available_flag = true;
#endif
}
