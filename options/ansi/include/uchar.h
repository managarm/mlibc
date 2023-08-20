#ifndef _UCHAR_H
#define _UCHAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/mbstate.h>
#include <bits/size_t.h>

#ifndef __cplusplus
typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;
#endif /* __cplusplus */

typedef struct __mlibc_mbstate mbstate_t;

#ifndef __MLIBC_ABI_ONLY

size_t c32rtomb(char *pmb, char32_t c32, mbstate_t *ps);
size_t mbrtoc32(char32_t *pc32, const char *pmb, size_t max, mbstate_t *ps);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _UCHAR_H */
