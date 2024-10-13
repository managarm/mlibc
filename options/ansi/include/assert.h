
#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/* NOTE: This is not ISO C. Declared in LSB */
__attribute__ ((__noreturn__)) void __assert_fail(const char *assertion, const char *file, unsigned int line,
		const char *function);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _ASSERT_H */

#include <mlibc-config.h>

#if __MLIBC_GLIBC_OPTION
#	include <bits/glibc/glibc_assert.h>
#endif

/* NOTE: [7.2] requires this be outside the include guard */
#ifdef NDEBUG

#undef assert
#define assert(ignore) ((void)0)

#else /* NDEBUG */

#undef assert
#define assert(assertion) ((void)((assertion) \
		|| (__assert_fail(#assertion, __FILE__, __LINE__, __func__), 0)))

#endif /* NDEBUG */

#ifndef __cplusplus
#undef static_assert
#define static_assert _Static_assert
#endif
