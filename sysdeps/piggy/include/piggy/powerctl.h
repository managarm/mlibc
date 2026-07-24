#ifndef _PIGGY_POWERCTL_H
#define _PIGGY_POWERCTL_H

#define POWERCTL_HALT       0x27baec8d
#define POWERCTL_REBOOT     0xce91fba2
#define POWERCTL_SHUTDOWN   0x19ba83ed

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int powerctl(int op);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _PIGGY_POWERCTL_H */
