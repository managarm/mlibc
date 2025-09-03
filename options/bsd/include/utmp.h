#ifndef  _UTMP_H
#define  _UTMP_H

#include <mlibc-config.h>

#if __MLIBC_LINUX_OPTION
#   include <bits/linux/linux_utmp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int login_tty(int __fd);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _UTMP_H */
