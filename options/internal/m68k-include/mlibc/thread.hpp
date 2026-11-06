#pragma once

#include <stdint.h>

namespace mlibc {

extern "C" void *__m68k_read_tp();

inline void *get_tp() {
	return __m68k_read_tp();
}

inline uintptr_t get_sp() {
	uintptr_t sp;
	asm volatile ("move.l %%sp, %0" : "=r"(sp));
	return sp;
}

} // namespace mlibc
