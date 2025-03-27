#ifndef MLIBC_RTLD_ABI
#define MLIBC_RTLD_ABI

#include <stddef.h>

#if defined(__x86_64__) || defined(__aarch64__) || defined(__i386__) || defined(__riscv) || defined (__m68k__) || defined(__loongarch64)

struct __abi_tls_entry {
	struct SharedObject *object;
	size_t offset;
};
static_assert(sizeof(__abi_tls_entry) == sizeof(size_t) * 2, "Bad __abi_tls_entry size");

extern "C" void *__dlapi_get_tls(struct __abi_tls_entry *);

#else
#error "Missing architecture specific code."
#endif

#if defined(__riscv)
constexpr inline unsigned long TLS_DTV_OFFSET = 0x800;
#elif defined(__x86_64__) || defined(__i386__) || defined(__aarch64__) || defined(__loongarch64)
constexpr inline unsigned long TLS_DTV_OFFSET = 0;
#elif defined(__m68k__)
constexpr inline unsigned long TLS_DTV_OFFSET = 0x8000;
#else
#error "Missing architecture specific code."
#endif

#endif // MLIBC_RTLD_ABI
