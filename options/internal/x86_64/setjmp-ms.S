.text

.globl __setjmp
.def __setjmp; .scl 2; .type 32; .endef
__setjmp:
	movq $0, 0x00(%rcx)
	mov %rbx, 0x08(%rcx)
	lea 8(%rsp), %rax
	mov %rax, 0x10(%rcx)
	mov %rbp, 0x18(%rcx)
	mov %rsi, 0x20(%rcx)
	mov %rdi, 0x28(%rcx)
	mov %r12, 0x30(%rcx)
	mov %r13, 0x38(%rcx)
	mov %r14, 0x40(%rcx)
	mov %r15, 0x48(%rcx)
	mov (%rsp), %rax
	mov %rax, 0x50(%rcx)
	stmxcsr 0x58(%rcx)
	fnstcw 0x5c(%rcx)
	movdqu %xmm6, 0x60(%rcx)
	movdqu %xmm7, 0x70(%rcx)
	movdqu %xmm8, 0x80(%rcx)
	movdqu %xmm9, 0x90(%rcx)
	movdqu %xmm10, 0xa0(%rcx)
	movdqu %xmm11, 0xb0(%rcx)
	movdqu %xmm12, 0xc0(%rcx)
	movdqu %xmm13, 0xd0(%rcx)
	movdqu %xmm14, 0xe0(%rcx)
	movdqu %xmm15, 0xf0(%rcx)

	test %r8d, %r8d
	jnz 1f
	xor %eax, %eax
	ret

1:
	jmp __sigsetjmp

.globl setjmp
.def setjmp; .scl 2; .type 32; .endef
.globl _setjmp
.def _setjmp; .scl 2; .type 32; .endef
setjmp:
_setjmp:
	xor %r8d, %r8d
	jmp __setjmp

.globl sigsetjmp
.def sigsetjmp; .scl 2; .type 32; .endef
sigsetjmp:
	mov $1, %r8d
	jmp __setjmp

.globl longjmp
.def longjmp; .scl 2; .type 32; .endef
.globl _longjmp
.def _longjmp; .scl 2; .type 32; .endef
longjmp:
_longjmp:
	mov %edx, %eax
	test %eax, %eax
	jnz 1f
	inc %eax

1:
	ldmxcsr 0x58(%rcx)
	fldcw 0x5c(%rcx)
	movdqu 0x60(%rcx), %xmm6
	movdqu 0x70(%rcx), %xmm7
	movdqu 0x80(%rcx), %xmm8
	movdqu 0x90(%rcx), %xmm9
	movdqu 0xa0(%rcx), %xmm10
	movdqu 0xb0(%rcx), %xmm11
	movdqu 0xc0(%rcx), %xmm12
	movdqu 0xd0(%rcx), %xmm13
	movdqu 0xe0(%rcx), %xmm14
	movdqu 0xf0(%rcx), %xmm15
	mov 0x08(%rcx), %rbx
	mov 0x18(%rcx), %rbp
	mov 0x20(%rcx), %rsi
	mov 0x28(%rcx), %rdi
	mov 0x30(%rcx), %r12
	mov 0x38(%rcx), %r13
	mov 0x40(%rcx), %r14
	mov 0x48(%rcx), %r15
	mov 0x10(%rcx), %rsp
	jmp *0x50(%rcx)
