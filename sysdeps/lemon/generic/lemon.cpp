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

	pid_t sys_getpid(){
		uint64_t _pid;
		syscall(SYS_GETPID, (uintptr_t)&_pid, 0, 0, 0, 0);

		pid_t pid = _pid;
		return pid;
	}
} 

extern "C" int __cxa_atexit(){
	return 0;
}

void __mlibc_do_finalize(){

}

extern "C" int lemon_spawn(const char* path){
	syscall(SYS_EXEC, (uintptr_t)path, 0, 0, 0, 0);
}
