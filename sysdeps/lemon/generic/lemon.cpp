#include <lemon/syscall.h>
#include <stddef.h>
#include <bits/ensure.h>
#include <bits/posix/pid_t.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

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
		sys_libc_log("libc panic!");
		__builtin_trap();
		for(;;);
	}

	pid_t sys_getpid(){
		uint64_t _pid;
		syscall(SYS_GETPID, (uintptr_t)&_pid, 0, 0, 0, 0);

		pid_t pid = _pid;
		return pid;
	}
	
	int sys_clock_get(int clock, time_t *secs, long *nanos) {
		uint64_t _secs, _millis;
		syscall(SYS_UPTIME, (uintptr_t)&_secs, (uintptr_t)&_millis, 0, 0, 0);

		*secs = _secs;
		*nanos = _millis * 1000000;
		return 0;
	}

	int sys_futex_wait(int *pointer, int expected){
		return 0;	
	}

	int sys_futex_wake(int *pointer) {
		return 0;
	}

	int sys_tcb_set(void* pointer){
		syscall(SYS_SET_FS_BASE, (uintptr_t)pointer, 0, 0, 0, 0);
		return 0;
	}

	int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
		__ensure(flags & MAP_ANONYMOUS);

		size_t sizePages = (size + 4096 - 1) / 4096;
		syscall(SYS_MMAP, (uintptr_t)window, sizePages, (uintptr_t)hint, 0, 0);

		if(!(*window))
			return -1;
		return 0;
	}
} 