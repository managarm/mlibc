#include <internal-config.h>
#include <mlibc/thread.hpp>
#include <mlibc/rtdl-abi.hpp>

struct __abi_tls_entry;

extern "C" void *__dlapi_get_tls(struct __abi_tls_entry *);

extern "C" void *__tls_get_addr(struct __abi_tls_entry *entry) {
	return __dlapi_get_tls(entry);
}

