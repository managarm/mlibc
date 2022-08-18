
.section .text
.global __mlibc_spawn_thread
.type __mlibc_spawn_thread, "function"
__mlibc_spawn_thread:
	xor %eax, %eax
	/* The rest of the args are already in the right registers,
	* only need to fixup rcx to r10
	*/
	mov %rcx, %r10
	mov $56, %al
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
.section .note.GNU-stack,"",%progbits

