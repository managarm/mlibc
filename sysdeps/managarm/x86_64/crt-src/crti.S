	.ident "x86_64-managarm-mlibc crti"

	.section .init
	.globl	_init
	.type	_init,@function
_init:
	push %rax

	.section .fini
	.globl	_fini
	.type	_fini,@function
_fini:
	push %rax

.section .note.GNU-stack,"",%progbits
