
#include <errno.h>
#include <stdint.h>
#include <sys/auxv.h>

#include <bits/ensure.h>

extern "C" uintptr_t *__dlapi_entrystack();

int peekauxval(unsigned long type, unsigned long *out) {
	// Find the auxiliary vector by skipping args and environment.
	auto aux = __dlapi_entrystack();
	aux += *aux + 1; // Skip argc and all arguments
	__ensure(!*aux);
	aux++;
	while(*aux) // Now, we skip the environment.
		aux++;
	aux++;

	// Parse the auxiliary vector.
	while(true) {
		auto value = aux + 1;
		if(*aux == AT_NULL) {
			errno = ENOENT;
			return -1;
		}else if(*aux == type) {
			*out = *value;
			return 0;
		}
		aux += 2;
	}
}

unsigned long getauxval(unsigned long type) {
	unsigned long value = 0;
	if(peekauxval(type, &value))
		return 0;
	return value;
}

// XXX(qookie):
// This is here because libgcc will call into __getauxval on glibc Linux
// (which is what it believes we are due to the aarch64-linux-gnu toolchain)
// in order to find AT_HWCAP to discover if LSE atomics are supported.
//
// This is not necessary on a custom Linux toolchain and is purely an artifact of
// using the host toolchain.

// __gnu_linux__ is the define checked by libgcc
#if defined(__aarch64__) && defined(__gnu_linux__)

extern "C" unsigned long __getauxval(unsigned long type) {
	unsigned long value = 0;
	if(peekauxval(type, &value))
		return 0;
	return value;
}

#endif
