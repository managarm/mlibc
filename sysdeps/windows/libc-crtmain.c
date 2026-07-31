#include "crt.h"

extern void __cxa_finalize(void *);
extern void *__dso_handle;
extern void __mlibc_windows_init_libc(void);

int DllMainCRTStartup(void *module, unsigned long reason, void *reserved) {
	(void)module;
	(void)reserved;

	if(reason == 1) {
		__mlibc_windows_init_libc();
		__mlibc_run_ctors(__CTOR_LIST__);
	}
	if(reason == 0)
		__cxa_finalize(&__dso_handle);
	return 1;
}
