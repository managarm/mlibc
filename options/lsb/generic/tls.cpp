#include <internal-config.h>
#include <mlibc/rtld-abi.hpp>

#if defined(__i386__)
	extern "C" __attribute__((regparm(1))) void *___tls_get_addr(struct __abi_tls_entry *entry) {
		return __dlapi_get_tls(entry);
	}
#else
	extern "C" void *__tls_get_addr(struct __abi_tls_entry *entry) {
		return __dlapi_get_tls(entry);
	}
#endif

