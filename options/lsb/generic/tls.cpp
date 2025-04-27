#include <internal-config.h>
#include <mlibc/thread.hpp>
#include <mlibc/rtld-abi.hpp>

#if (defined(__riscv) || defined(__m68k__) || defined(__loongarch64)) && defined(MLIBC_STATIC_BUILD)
	// On RISC-V, m68k and loongarch64, linker optimisation is not guaranteed and so we may
	// still get calls to this function in statically linked binaries.
	// TODO: This will break dlopen calls from statically linked programs.
	extern "C" void *__tls_get_addr(struct __abi_tls_entry *entry) {
		Tcb *tcbPtr = mlibc::get_current_tcb();
		auto dtvPtr = reinterpret_cast<char *>(tcbPtr->dtvPointers[0]);
		return reinterpret_cast<void *>(dtvPtr + entry->offset + TLS_DTV_OFFSET);
	}
#elif defined(__i386__)
	extern "C" __attribute__((regparm(1))) void *___tls_get_addr(struct __abi_tls_entry *entry) {
		return __dlapi_get_tls(entry);
	}
#else
	extern "C" void *__tls_get_addr(struct __abi_tls_entry *entry) {
		return __dlapi_get_tls(entry);
	}
#endif

