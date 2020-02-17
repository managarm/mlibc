#include <lemon/syscall.h>
#include <stddef.h>
#include <bits/ensure.h>
#include <bits/posix/pid_t.h>

namespace mlibc{

	void sys_exit(int status){
		syscall(SYS_EXIT, status, 0, 0, 0, 0);
	}

	int sys_anon_allocate(size_t size, void **pointer) {
		// Make sure to only allocate whole pages
		__ensure(!(size & 0xFFF));
		syscall(SYS_ALLOC, ((size + 0xFFF) & ~static_cast<size_t>(0xFFF)) >> 12, (uintptr_t)pointer, 0, 0, 0);

		if (!(*pointer))
		    return -1;
		return 0;
	}

	int sys_anon_free(void *pointer, size_t size) {
		return 0; // Not implemented
	}

	void sys_libc_panic(){
		__builtin_trap();
		for(;;);
	}

	pid_t sys_getpid(){
		uint64_t _pid;
		syscall(SYS_GETPID, (uintptr_t)&_pid, 0, 0, 0, 0);

		pid_t pid = _pid;
		return pid;
	}
} 

// Temporary - Remove once LSB is linked into the static library
extern "C" int __cxa_atexit(){
	return 0;
}

// Temporary - Remove once LSB is linked into the static library
void __mlibc_do_finalize(){

}
