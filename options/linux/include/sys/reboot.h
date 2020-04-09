#ifndef MLIBC_SYS_REBOOT_H
#define MLIBC_SYS_REBOOT_H

#include <abi-bits/reboot.h>

#ifdef __cplusplus
extern "C" {
#endif

int reboot(int arg);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_SYS_REBOOT_H
