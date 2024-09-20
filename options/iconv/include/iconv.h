#ifndef _ICONV_H
#define _ICONV_H

#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *iconv_t;

#ifndef __MLIBC_ABI_ONLY

size_t iconv(iconv_t __cd, char **__restrict __inbuf, size_t *__restrict __inbytesleft,
		char **__restrict __outbuf, size_t *__restrict __outbytesleft);
int iconv_close(iconv_t __cd);
iconv_t iconv_open(const char *__tocode, const char *__fromcode);

#endif /* !__MLIBC_ABI_ONLY */


#ifdef __cplusplus
}
#endif

#endif
