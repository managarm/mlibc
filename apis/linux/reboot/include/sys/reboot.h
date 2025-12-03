#ifndef _MLIBC_SYS_REBOOT_H
#define _MLIBC_SYS_REBOOT_H

#include <abi-bits/linux/reboot.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int reboot(int __arg);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_SYS_REBOOT_H */
