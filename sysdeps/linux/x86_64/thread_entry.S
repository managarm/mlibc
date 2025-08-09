#include "mlibc-asm/helpers.h"
#include "syscallnos.h"

.section .text

PROC_START(__mlibc_spawn_thread)
	xor %eax, %eax
	/* The rest of the args are already in the right registers,
	* only need to fixup rcx to r10
	*/
	mov %rcx, %r10
	mov $__NR_clone, %rax
	syscall
	test %eax, %eax
	jnz 1f
	xor %ebp, %ebp
	pop %rdi
	pop %rsi
	call __mlibc_enter_thread
	hlt
1:
	ret
PROC_END(__mlibc_spawn_thread)

GNU_STACK_NOTE()

