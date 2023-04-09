#ifndef _ICONV_H
#define _ICONV_H

#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *iconv_t;

#ifndef __MLIBC_ABI_ONLY

size_t iconv(iconv_t, char **__restrict, size_t *__restrict, char **__restrict, size_t *__restrict);
int iconv_close(iconv_t);
iconv_t iconv_open(const char *, const char *);

#endif /* !__MLIBC_ABI_ONLY */


#ifdef __cplusplus
}
#endif

#endif
