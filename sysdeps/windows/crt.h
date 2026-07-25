#ifndef MLIBC_SYSDEPS_WINDOWS_CRT_H
#define MLIBC_SYSDEPS_WINDOWS_CRT_H

typedef unsigned long long mlibc_crt_uintptr_t;

#ifdef __cplusplus
extern "C" {
#endif

extern mlibc_crt_uintptr_t __CTOR_LIST__[];
extern void __mlibc_run_ctors(mlibc_crt_uintptr_t *);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_SYSDEPS_WINDOWS_CRT_H
