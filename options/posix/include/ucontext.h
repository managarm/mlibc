#ifndef _UCONTEXT_H
#define _UCONTEXT_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef __MLIBC_ABI_ONLY

int getcontext(ucontext_t *__uctx);
int setcontext(const ucontext_t *__uctx);
void makecontext(ucontext_t *__uctx, void (*__fn)(void), int __argc, ...);
int swapcontext(ucontext_t *__uctx, const ucontext_t *__newctx);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _UCONTEXT_H */
