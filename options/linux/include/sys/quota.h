#ifndef	_SYS_QUOTA_H
#define	_SYS_QUOTA_H

#include <sys/types.h>

#define SUBCMDMASK 0x00ff
#define SUBCMDSHIFT 8
#define QCMD(cmd, type) (((cmd) << SUBCMDSHIFT) | ((type) & SUBCMDMASK))

#ifdef __cplusplus
extern "C" {
#endif

struct dqblk {
    uint64_t dqb_bhardlimit;
    uint64_t dqb_bsoftlimit;
    uint64_t dqb_curspace;
    uint64_t dqb_ihardlimit;
    uint64_t dqb_isoftlimit;
    uint64_t dqb_curinodes;
    uint64_t dqb_btime;
    uint64_t dqb_itime;
    uint32_t dqb_valid;
};

#ifndef __MLIBC_ABI_ONLY

int quotactl(int __cmd, const char *__special, int __id, __caddr_t __addr);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_QUOTA_H */
