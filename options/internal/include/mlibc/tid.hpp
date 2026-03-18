#pragma once

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread.hpp>

namespace mlibc {
	inline unsigned int this_tid() {
		// During RTLD initialization, we don't have a TCB.
		if (mlibc::tcb_available_flag) {
			auto tcb = get_current_tcb();
			return tcb->tid;
		} else if (mlibc::IsImplemented<FutexTid>) {
			return mlibc::sysdep_or_panic<FutexTid>();
		} else {
			return 1;
		}
	}
} // namespace mlibc
