#ifndef _MLIBC_BSD_UNISTD_H
#define _MLIBC_BSD_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE)
int issetugid(void);
#endif /* defined(_DEFAULT_SOURCE) || defined(_BSD_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_BSD_UNISTD_H */
