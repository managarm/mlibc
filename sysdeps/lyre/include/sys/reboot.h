#ifndef MLIBC_SYS_REBOOT_H
#define MLIBC_SYS_REBOOT_H

#include <abi-bits/reboot.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int reboot(int arg);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_SYS_REBOOT_H */
