
.type __setjmp, "function"
__setjmp:
	mov %rbx, 0x00(%rdi)
	mov %rbp, 0x08(%rdi)
	mov %r12, 0x10(%rdi)
	mov %r13, 0x18(%rdi)
	mov %r14, 0x20(%rdi)
	mov %r15, 0x28(%rdi)

	lea 8(%rsp), %rax # rsp before return rip is pushed
	mov %rax, 0x30(%rdi)
	mov (%rsp), %rax # return rip
	mov %rax, 0x38(%rdi)

	test %rdx, %rdx
	jnz 1f
	xor %rax, %rax
	ret

1:
	jmp __sigsetjmp

.global setjmp
.type setjmp, "function"
.global _setjmp
.type _setjmp, "function"
setjmp:
_setjmp:
	xor %rdx, %rdx
	jmp __setjmp

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
	mov $1, %rdx
	jmp __setjmp

.global longjmp
.type longjmp, "function"
.global _longjmp
.type _longjmp, "function"
longjmp:
_longjmp:
	mov 0x00(%rdi), %rbx
	mov 0x08(%rdi), %rbp
	mov 0x10(%rdi), %r12
	mov 0x18(%rdi), %r13
	mov 0x20(%rdi), %r14
	mov 0x28(%rdi), %r15

	mov  %rsi, %rax
	test %rax, %rax
	jnz 1f
	inc %rax
1:
	mov  0x30(%rdi), %rsp
	jmp  *0x38(%rdi)
.section .note.GNU-stack,"",%progbits

