#include <lemon/syscall.h>
#include <stddef.h>
#include <bits/ensure.h>

namespace mlibc{
	void sys_exit(int status){
		syscall(SYS_EXIT, 0, 0, 0, 0, 0);
	}

	int sys_anon_allocate(size_t size, void **pointer) {
		// Make sure to only allocate whole pages
		__ensure(!(size & 0xFFF));
		syscall(SYS_ALLOC, (((size / 0x1000)*0x1000 < size) ? (size / 0x1000) + 1 : (size / 0x1000)), (uintptr_t)pointer, 0, 0, 0);

		if (!(*pointer))
		    return -1;
		return 0;
	}

	int sys_anon_free(void *pointer, size_t size) {
		return 0; // Not implemented
	}

	void sys_libc_panic(){
		*((int*)0xffffDEADDEADDEAD) = 1; // Force Page Fault
		for(;;);
	}
} 

extern "C" int __cxa_atexit(){
	return 0;
}

void __mlibc_do_finalize(){

}
