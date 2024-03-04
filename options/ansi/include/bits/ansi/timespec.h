
#ifndef MLIBC_TIMESPEC_H
#define MLIBC_TIMESPEC_H

#include <bits/ansi/time_t.h>
#include <bits/field-padding.h>

// Equivalent of timespec64 in glibc.
// Should be used only with 64-bit syscalls
// or with appropriate compat syscalls.
struct timespec {
	time_t tv_sec;
	// tv_nspec is required to be long by the C standard.
	// However linux kernel expects long long. So we add padding.
	__MLIBC_FIELD_PADDED(long, tv_nsec, long long);
};

#endif // MLIBC_TIMESPEC_H

