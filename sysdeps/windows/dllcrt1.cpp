#include "crt.h"

extern "C" void __cxa_finalize(void *);
extern "C" void *__dso_handle;

extern "C"
__attribute__((weak))
int DllMain(void *module, unsigned long reason, void *reserved) {
	(void)module;
	(void)reason;
	(void)reserved;
	return 1;
}

extern "C"
int DllMainCRTStartup(void *module, unsigned long reason, void *reserved) {
	if(reason == 1)
		__mlibc_run_ctors(__CTOR_LIST__);

	int result = DllMain(module, reason, reserved);
	if(reason == 0)
		__cxa_finalize(&__dso_handle);
	return result;
}
