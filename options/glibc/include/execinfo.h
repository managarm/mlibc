#ifndef _EXECINFO_H
#define _EXECINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int backtrace(void **__buffer, int __size);
char **backtrace_symbols(void *const *__buffer, int __size);
void backtrace_symbols_fd(void *const *__buffer, int __size, int __fd);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif
