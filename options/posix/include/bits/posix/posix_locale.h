#ifndef MLIBC_POSIX_LOCALE_H
#define MLIBC_POSIX_LOCALE_H

#include <bits/posix/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

locale_t newlocale(int __category_mask, const char *__locale, locale_t __base);
void freelocale(locale_t __locobj);
locale_t uselocale(locale_t __locobj);
locale_t duplocale(locale_t __locobj);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_POSIX_LOCALE_H */
