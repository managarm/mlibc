#ifndef MLIBC_GLIBC_STDLIB_H
#define MLIBC_GLIBC_STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*comparison_fn_t) (const void *__a, const void *__b);

#ifndef __MLIBC_ABI_ONLY

int rpmatch(const char *__resp);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_GLIBC_STDLIB_H */
