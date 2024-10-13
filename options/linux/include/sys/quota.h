#ifndef	_SYS_QUOTA_H
#define	_SYS_QUOTA_H

#include <sys/types.h>

#define SUBCMDMASK 0x00ff
#define SUBCMDSHIFT 8
#define QCMD(cmd, type) (((cmd) << SUBCMDSHIFT) | ((type) & SUBCMDMASK))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int quotactl(int __cmd, const char *__special, int __id, caddr_t __addr);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_QUOTA_H */
