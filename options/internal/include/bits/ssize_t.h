
#ifndef MLIBC_SSIZE_T_H
#define MLIBC_SSIZE_T_H

/* TODO: use ptrdiff_t instead? */
#if __UINTPTR_MAX__ == __UINT64_MAX__
typedef long ssize_t;
#elif __UINTPTR_MAX__ == __UINT32_MAX__
typedef int ssize_t;
#else
#error "unsupported architecture"
#endif

#endif /* MLIBC_SSIZE_T_H */

