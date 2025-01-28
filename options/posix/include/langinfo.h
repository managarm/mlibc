
#ifndef _LANGINFO_H
#define _LANGINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/posix/locale_t.h>
#include <bits/nl_item.h>

#ifndef __MLIBC_ABI_ONLY

char *nl_langinfo(nl_item __item);
char *nl_langinfo_l(nl_item __item, locale_t __loc);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _LANGINFO_H */

