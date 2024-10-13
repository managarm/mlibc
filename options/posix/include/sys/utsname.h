
#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

#include <abi-bits/utsname.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int uname(struct utsname *__name);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_UTSNAME_H */

