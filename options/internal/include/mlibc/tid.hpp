#pragma once

#include <mlibc/thread.hpp>

namespace mlibc {
	inline unsigned int this_tid() {
		auto tcb = get_current_tcb();
		return tcb->tid;
	}
}
