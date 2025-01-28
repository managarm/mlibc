#ifndef _BSD_UNISTD_H
#define _BSD_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef __MLIBC_ABI_ONLY

void *sbrk(intptr_t __increment);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BSD_UNISTD_H */
