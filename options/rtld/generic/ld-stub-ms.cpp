// stub rtld functions for the MS ABI.
#include <stdint.h>

#include <mlibc/rtld-config.hpp>
#include <mlibc/rtld-abi.hpp>
#include <mlibc/tcb.hpp>

extern "C" void *__rtld_allocateTcb() {
	return nullptr;
}

extern "C" const mlibc::RtldConfig &__dlapi_get_config() {
	static constinit mlibc::RtldConfig config = {};
	return config;
}

extern "C" void __cxa_finalize(void *);
extern "C" uintptr_t __DTOR_LIST__[];

extern "C" void __dlapi_exit() {
	static bool destructors_run;
	if(destructors_run)
		return;
	destructors_run = true;

	// PE collects compiler-generated destructors in a sentinel-terminated
	// list. In a static executable this is the executable's list and in a shared
	// build this stub is part of libc.dll and therefore sees libc's list.
	auto entry = __DTOR_LIST__;
	if(*entry == static_cast<uintptr_t>(-1))
		entry++;
	while(*entry) {
		auto function = reinterpret_cast<void (*)()>(*entry++);
		function();
	}

	__cxa_finalize(nullptr);
}

extern "C" void *__dlapi_get_tls(struct __abi_tls_entry *) {
	return nullptr;
}
