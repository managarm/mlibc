#ifndef MLIBC_RTDL_ABI
#define MLIBC_RTDL_ABI

#include <stdint.h>

#if defined(__x86_64__) || defined(__aarch64__) || defined(__riscv)

struct __abi_tls_entry {
	struct SharedObject *object;
	uint64_t offset;
};
static_assert(sizeof(__abi_tls_entry) == 16, "Bad __abi_tls_entry size");

extern "C" void *__dlapi_get_tls(struct __abi_tls_entry *);

#else
#error "Missing architecture specific code."
#endif

#if defined(__riscv)
constexpr inline unsigned long TLS_DTV_OFFSET = 0x800;
#elif defined(__x86_64__) || defined(__aarch64__)
constexpr inline unsigned long TLS_DTV_OFFSET = 0;
#else
#error "Missing architecture specific code."
#endif

#endif // MLIBC_RTDL_ABI
