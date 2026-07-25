.text

.globl feclearexcept
.def feclearexcept; .scl 2; .type 32; .endef
feclearexcept:
	mov %ecx, %edx
	and $0x3f, %edx
	fnstsw %ax
	test %eax, %edx
	jz 1f
	fnclex

1:
	stmxcsr 8(%rsp)
	and $0x3f, %eax
	or %eax, 8(%rsp)
	test %edx, 8(%rsp)
	jz 2f
	not %edx
	and %edx, 8(%rsp)
	ldmxcsr 8(%rsp)

2:
	xor %eax, %eax
	ret

.globl feraiseexcept
.def feraiseexcept; .scl 2; .type 32; .endef
feraiseexcept:
	and $0x3f, %ecx
	stmxcsr 8(%rsp)
	or %ecx, 8(%rsp)
	ldmxcsr 8(%rsp)
	xor %eax, %eax
	ret

.globl __fesetround
.def __fesetround; .scl 2; .type 32; .endef
__fesetround:
	fnstcw 8(%rsp)
	andb $0xf3, 9(%rsp)
	or %ch, 9(%rsp)
	fldcw 8(%rsp)
	stmxcsr 8(%rsp)
	shl $3, %ch
	andb $0x9f, 9(%rsp)
	or %ch, 9(%rsp)
	ldmxcsr 8(%rsp)
	xor %eax, %eax
	ret

.globl fegetround
.def fegetround; .scl 2; .type 32; .endef
fegetround:
	stmxcsr 8(%rsp)
	mov 8(%rsp), %eax
	shr $3, %eax
	and $0xc00, %eax
	ret

.globl fegetenv
.def fegetenv; .scl 2; .type 32; .endef
fegetenv:
	fnstenv (%rcx)
	stmxcsr 28(%rcx)
	xor %eax, %eax
	ret

.globl fesetenv
.def fesetenv; .scl 2; .type 32; .endef
fesetenv:
	cmp $-1, %rcx
	je 1f
	fldenv (%rcx)
	ldmxcsr 28(%rcx)
	xor %eax, %eax
	ret

1:
	movq $0x037f, 8(%rsp)
	movq $0xffff, 16(%rsp)
	movq $0, 24(%rsp)
	movl $0, 32(%rsp)
	fldenv 8(%rsp)
	movl $0x1f80, 8(%rsp)
	ldmxcsr 8(%rsp)
	xor %eax, %eax
	ret

.globl fetestexcept
.def fetestexcept; .scl 2; .type 32; .endef
fetestexcept:
	and $0x3f, %ecx
	stmxcsr 8(%rsp)
	mov 8(%rsp), %edx
	fnstsw %ax
	or %edx, %eax
	and %ecx, %eax
	ret
