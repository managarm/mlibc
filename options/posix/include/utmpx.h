
#ifndef  _UTMPX_H
#define  _UTMPX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/utmp-defines.h>
#include <abi-bits/utmpx.h>

#ifndef __MLIBC_ABI_ONLY

void endutxent(void);
struct utmpx *getutxent(void);
struct utmpx *getutxid(const struct utmpx *__id);
struct utmpx *getutxline(const struct utmpx *__ut);
struct utmpx *pututxline(const struct utmpx *__line);
void setutxent(void);

/* extensions */
void updwtmpx(const char *__wtmp_file, const struct utmpx *__ut);
int utmpxname(const char *__file);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _UTMPX_H */
