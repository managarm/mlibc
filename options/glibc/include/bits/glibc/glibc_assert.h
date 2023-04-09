#ifndef MLIBC_GLIBC_ASSERT_H
#define MLIBC_GLIBC_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

void __assert_fail_perror(int errno, const char *file, unsigned int line,
		const char *function);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_GLIBC_ASSERT_H */

#ifdef NDEBUG

#undef assert_perror
#define assert_perror(ignore) ((void)0)

#else /* NDEBUG */

#undef assert_perror
#define assert_perror(errno) (!(errno) \
		|| (__assert_fail_perror((errno), __FILE__, __LINE__, __func__), 0))

#endif /* NDEBUG */
