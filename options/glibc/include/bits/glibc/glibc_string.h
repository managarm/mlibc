#ifndef _MLIBC_GLIBC_STRING_H
#define _MLIBC_GLIBC_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

#if defined(_GNU_SOURCE)
int strverscmp(const char *__l0, const char *__r0);
#endif

#if defined(_DEFAULT_SOURCE)
void *mempcpy(void *__dest, const void *__src, size_t __size);

void explicit_bzero(void *__s, size_t __len);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_GLIBC_STRING_H */

