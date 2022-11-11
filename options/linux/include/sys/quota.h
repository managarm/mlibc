#ifndef	_SYS_QUOTA_H
#define	_SYS_QUOTA_H

#include <sys/types.h>

#define SUBCMDMASK 0x00ff
#define SUBCMDSHIFT 8
#define QCMD(cmd, type) (((cmd) << SUBCMDSHIFT) | ((type) & SUBCMDMASK))

#ifdef __cplusplus
extern "C" {
#endif

int quotactl(int cmd, const char *special, int id, caddr_t addr);

#ifdef __cplusplus
}
#endif

#endif // _SYS_QUOTA_H
