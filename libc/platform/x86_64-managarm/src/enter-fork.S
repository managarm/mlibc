
.global __mlibc_enterFork
.hidden __mlibc_enterFork
__mlibc_enterFork:
	push %rbx
	push %rbp
	push %r12
	push %r13
	push %r14
	push %r15
	lea .Lin_child(%rip), %rdi
	mov %rsp, %rsi
.hidden __mlibc_doFork
	call __mlibc_doFork
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rbp
	pop %rbx
	ret

.Lin_child:
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rbp
	pop %rbx
.hidden __mlibc_fixForkedChild
	call __mlibc_fixForkedChild
	xor %rax, %rax
	ret

