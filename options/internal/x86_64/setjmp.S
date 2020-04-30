
.global setjmp
.type setjmp, "function"
setjmp:
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
	
	mov $0, %rax
	ret

.global longjmp
.type longjmp, "function"
longjmp:
	mov 0x00(%rdi), %rbx
	mov 0x08(%rdi), %rbp
	mov 0x10(%rdi), %r12
	mov 0x18(%rdi), %r13
	mov 0x20(%rdi), %r14
	mov 0x28(%rdi), %r15

	mov %rsi, %rax
	mov 0x30(%rdi), %rsp
	jmp *0x38(%rdi)

.global sigsetjmp
.type sigsetjmp, "function"
sigsetjmp:
	ret

.global siglongjmp
.type siglongjmp, "function"
siglongjmp:
	ret

