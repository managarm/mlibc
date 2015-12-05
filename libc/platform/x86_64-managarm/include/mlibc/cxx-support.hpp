
#ifndef MLIBC_CXX_SUPPORT_HPP
#define MLIBC_CXX_SUPPORT_HPP

#include <stddef.h>

#pragma GCC visibility push(hidden)

inline void *operator new(size_t length, void *pointer) {
	return pointer;
}

#pragma GCC visibility pop

#endif // MLIBC_CXX_SUPPORT_HPP

