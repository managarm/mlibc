
#ifndef MLIBC_TIMESPEC_H
#define MLIBC_TIMESPEC_H

#include <bits/ansi/time_t.h>
#include <bits/field-padding.h>

struct timespec {
	time_t tv_sec;
	__MLIBC_FIELD_PADDED(long, tv_nsec, long long);
};

#endif // MLIBC_TIMESPEC_H

