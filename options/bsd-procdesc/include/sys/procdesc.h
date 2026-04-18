#ifndef _SYS_PROCDESC_H
#define _SYS_PROCDESC_H

#include <abi-bits/procdesc.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

struct __wrusage;

pid_t pdfork(int *__fdp, int __flags);
int pdkill(int __fd, int __sig);
int pdgetpid(int __fd, pid_t *__pidp);
int pdwait(int __fd, int *__status, int __options, struct __wrusage *__wrusage, siginfo_t *__siginfo);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_PROCDESC_H */
